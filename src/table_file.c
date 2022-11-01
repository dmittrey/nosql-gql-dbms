#include "../include/table_file.h"
#include <inttypes.h>

// TODO Session logs? (f.e. Not opened files etc.)

static struct TransactionResult performFileOpen(file_path_t file_path, const char *op_flag);

static struct TransactionResult openFile(file_path_t file_path);

static struct TransactionResult createOrBlankFile(file_path_t file_path);

static enum PerformStatus writeHeader(FILE *file, struct Header *header, fileoff_t location);

static enum PerformStatus readHeader(FILE *file, struct Header *header, fileoff_t location);

static struct Document *readDocumentFromItem(table_file_t file);

/* File manipulation */

struct TransactionResult openOrCreateTableStoreFile(file_path_t file_path)
{
    struct TransactionResult t = openFile(file_path);

    // If we opened file => exist and we can wrap into transaction
    if (IS_INVALID_TRANSACTION(t))
    {
        t = createOrBlankFile(file_path);

        if (IS_INVALID_TRANSACTION(t))
        {
            INFO("File cannot be opened!\n");
            return TRANSACTION_FAILED();
        }

        INFO("File created!\n");
    }

    INFO("File opened!\n");

    return t;
}

struct TransactionResult closeTableStoreFile(table_file_t *file)
{
    fclose(to_FILE(file));
    INFO("File closed!");
    return TRANSACTION_FAILED();
}

/* Section manipulation */

/*
Creates document section at the place, where is buffer ptr
*/
enum PerformStatus createDocumentsSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Documents *prev_section // Can be null if section is first
)
{
    SEEK_OR_RETURN_FAIL(table_file, location);

    // Set prev next ptr to next
    if (prev_section != NULL)
    {
        prev_section->header->next = location;
    }

    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->free_space = DOCUMENTS_SECTION_SIZE - sizeof(header);
    header->next = NULL_NEXT;
    header->last_item_ptr = location + sizeof(header);
    header->first_record_ptr = location + DOCUMENTS_SECTION_SIZE - 1;

    return writeHeader(table_file, header, location);
}

enum PerformStatus createExtentSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Extent *prev_section // Can be null if section is first
)
{
    FILE *file = to_FILE(table_file);

    SEEK_OR_RETURN_FAIL(file, location);

    // Set prev next ptr to next
    if (prev_section != NULL)
    {
        prev_section->header->next = location;
    }

    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->free_space = EXTENT_SECTION_SIZE - sizeof(header);
    header->next = NULL_NEXT;
    header->last_item_ptr = location + sizeof(header);
    header->first_record_ptr = location + EXTENT_SECTION_SIZE - 1;

    return writeHeader(file, header, location);
}

/* Data manipulation */

// TODO check perform status
enum PerformStatus readDocumentNode(table_file_t *file, struct string_t doc_name, struct Document *res_document)
{
    fileoff_t cur_sec_location = 0;
    struct Header *documents_header;

    do
    {
        // Migrate to start of table to first document
        fseek(file, cur_sec_location, SEEK_SET);

        documents_header = my_malloc(struct Header);

        // Read header to fetch next section offset
        enum PerformStatus result = readHeader(file, documents_header, 0);
        if (result != 0)
        {
            return result;
        }

        // Iterate over items(name_lenght, name_ptr, first_node_addr)(24 byte)
        // TODO Refactor(add new struct to represent items and names or refactor physical representation)
        for (size_t i = cur_sec_location + sizeof(documents_header); i < documents_header->last_item_ptr; i += 24)
        {
            // Start of new item and read address of record
            fseek(file, i, SEEK_SET);

            // Read new document
            struct Document *cur_document = readDocumentFromItem(file);

            // TODO optimize feetching document?? (Check name and after fetch all data)
            if (compare_string_t(*(cur_document->name), doc_name))
            {
                res_document = cur_document;
                return OK;
            }
        }

        if (documents_header->next != NULL_NEXT)
        {
            cur_sec_location = documents_header->next;
        }
    } while (documents_header->next != NULL_NEXT);
}

/*
Checks can we insert document into section

If we can, returns offset, where we can insert document text
Else returns 0
*/
static inline fileoff_t checkForFreeSpace(struct Document *doc_to_insert, struct Header *insert_sec_header)
{
    // Item + doc physical representation
    size_t size_of_doc_block = sizeof(doc_to_insert) + ITEM_SIZE;

    if (insert_sec_header->free_space >= size_of_doc_block)
    {
        return insert_sec_header->first_record_ptr - doc_to_insert->name->count * sizeof(char) + 1;
    }
    else
    {
        return 0;
    }
}

/*
Firstly, we check free space and decide to put it in this section or create new

Secondly, we going to end of items and put information about document.
Document's name we put at the end of section before older names.
*/
enum PerformStatus insertDocumentNode(table_file_t *file, struct Document *document)
{
    fileoff_t cur_sec_location = 0;
    struct Header *documents_header;

    do
    {
        // Migrate to start of table to first document
        fseek(file, cur_sec_location, SEEK_SET);

        documents_header = my_malloc(struct Header);

        // Read header to fetch next section offset
        enum PerformStatus result = readHeader(file, documents_header, 0);
        if (result != 0)
        {
            return result;
        }

        fileoff_t doc_name_ptr = checkForFreeSpace(document, documents_header);
        if (doc_name_ptr != 0)
        {
            size_t wroten = 0;
            fseek(file, documents_header->last_item_ptr, SEEK_SET);

            wroten = fwrite(document->name->count, sizeof(document->name->count), 1, file);
            if (wroten != 1)
                return FAILED; // fwrite returns successfully written objects

            wroten = fwrite(doc_name_ptr, sizeof(fileoff_t), 1, file);
            if (wroten != 1)
                return FAILED;

            // TODO Calculate случай когда имеем ноды
            wroten = fwrite(0, sizeof(ITEM_SIZE), 1, file);
            if (wroten != 1)
                return FAILED;

            wroten = fwrite(document->name->val, document->name->count * sizeof(char), 1, file);
            if (wroten != 1)
                return FAILED;

            return OK;
        }

    } while (documents_header->next != NULL_NEXT);
}

enum PerformStatus updateDocumentNode(table_file_t *file, struct Document *prev_document, struct Document *new_document);

/*
Change ptr's in the header(We will override data, because of it wi will not touch data)
*/
enum PerformStatus deleteDocumentNode(table_file_t *file, struct string_t* doc_name)
{
    fileoff_t cur_sec_location = 0;
    struct Header *documents_header;

    do
    {
        // Migrate to start of table to first document
        fseek(file, cur_sec_location, SEEK_SET);

        documents_header = my_malloc(struct Header);

        // Read header to fetch next section offset
        enum PerformStatus result = readHeader(file, documents_header, 0);
        if (result != 0)
        {
            return result;
        }

        // Iterate over items(name_lenght, name_ptr, first_node_addr)(24 byte)
        // TODO Refactor(add new struct to represent items and names or refactor physical representation)
        for (size_t i = cur_sec_location + sizeof(documents_header); i < documents_header->last_item_ptr; i += 24)
        {
            // Start of new item and read address of record
            fseek(file, i, SEEK_SET);

            // Read new document
            struct Document *cur_document = readDocumentFromItem(file);

            // TODO optimize feetching document?? (Check name and after fetch all data)
            if (compare_string_t(*(cur_document->name), doc_name))
            {
                //TODO add fseek
                size_t wroten = fwrite(documents_header->last_item_ptr - 8, sizeof(documents_header->last_item_ptr), 1, file);
                if (wroten != 1)
                    return FAILED; // fwrite returns successfully written objects

            //TODO add fseek
                size_t wroten = fwrite(documents_header->first_record_ptr + cur_document->name->count, sizeof(documents_header->first_record_ptr), 1, file);
                if (wroten != 1)
                    return FAILED; // fwrite returns successfully written objects

                return OK;
            }
        }

        if (documents_header->next != NULL_NEXT)
        {
            cur_sec_location = documents_header->next;
        }
    } while (documents_header->next != NULL_NEXT);
}

/*
We going forward over Document entites and searching

If we detect end of Documents section we go on next Documents section
*/

/* Static functions */

static struct TransactionResult performFileOpen(file_path_t file_path, const char *op_flag)
{
    FILE *open_file = fopen(file_path, op_flag);

    return (open_file != NULL) ? TRANSACTION_OK(open_file) : TRANSACTION_FAILED();
}

static struct TransactionResult openFile(file_path_t file_path)
{
    return performFileOpen(file_path, READ_WRITE);
}

static struct TransactionResult createOrBlankFile(file_path_t file_path)
{
    return performFileOpen(file_path, READ_WRITE_NEW);
}

static enum PerformStatus writeHeader(FILE *file, struct Header *header, fileoff_t location)
{
    size_t wroten = 0;

    wroten = fwrite(&header->free_space, sizeof(header->free_space), 1, file);
    if (wroten != 1)
        return FAILED; // fwrite returns successfully written objects

    wroten = fwrite(&header->next, sizeof(header->next), 1, file);
    if (wroten != 1)
        return FAILED;

    wroten = fwrite(&header->last_item_ptr, sizeof(header->last_item_ptr), 1, file);
    if (wroten != 1)
        return FAILED;

    wroten = fwrite(&header->first_record_ptr, sizeof(header->first_record_ptr), 1, file);
    if (wroten != 1)
        return FAILED;

    return OK;
}

static enum PerformStatus readHeader(FILE *file, struct Header *header, fileoff_t location)
{
    size_t readen = 0;

    readen = fread(&header->free_space, sizeof(header->free_space), 1, file);
    if (readen != 1)
        return FAILED; // fwrite returns successfully written objects

    readen = fread(&header->next, sizeof(header->next), 1, file);
    if (readen != 1)
        return FAILED;

    readen = fread(&header->last_item_ptr, sizeof(header->last_item_ptr), 1, file);
    if (readen != 1)
        return FAILED;

    readen = fread(&header->first_record_ptr, sizeof(header->first_record_ptr), 1, file);
    if (readen != 1)
        return FAILED;

    return OK;
}

/*
We lose file seek in this method
*/
static struct Document *readDocumentFromItem(table_file_t file)
{
    struct Document *document = my_malloc(struct Document);
    document->name = my_malloc(struct string_t);

    fread(document->name->count, sizeof(uint64_t), 1, file);

    fileoff_t name_fileoff;
    fread(&name_fileoff, sizeof(name_fileoff), 1, file);

    fread(document->first_node, sizeof(document->first_node), 1, file);

    // Migrate to record address
    fseek(file, name_fileoff, SEEK_SET);
    fread(document->name->val, sizeof(char), document->name->count, file);

    return document;
}