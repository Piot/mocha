#ifndef mocha_object_type_h
#define mocha_object_type_h

typedef enum mocha_object_type {
	mocha_object_type_nil,
	mocha_object_type_list,
	mocha_object_type_map,
	mocha_object_type_vector,
	mocha_object_type_integer,
	mocha_object_type_string,
	mocha_object_type_character,
	mocha_object_type_keyword,
	mocha_object_type_true,
	mocha_object_type_symbol,
	mocha_object_type_function,
	mocha_object_type_internal_function,
	mocha_object_type_blob,
	mocha_object_type_recur,
	mocha_object_type_eval,
	mocha_object_type_context
} mocha_object_type;

#endif
