#pragma once

#include <assert.h>

void do_tests();

void test_header();

void test_extents_write();
void test_extents_read();
void test_extents_delete();
void test_extents_update();

void test_extents_load();

void test_types_write();
void test_types_delete();
void test_types_read();

void test_types_load();
void test_types_find();

void test_file_add_type();
void test_file_delete_type();

void test_file_write();