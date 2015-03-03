#ifndef CSV_INFERENCE_H
#define CSV_INFERENCE_H

//enum field_type
//{
//  string_type,
//  numeric_type
//};

//enum field_type infer_type(const char * field, int len);

int should_quote(const char * field, int len, char delim);

#endif /* CSV_INFERENCE_H */
