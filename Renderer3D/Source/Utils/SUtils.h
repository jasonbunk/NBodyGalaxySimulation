#ifndef ___S_UTILS_HPP_____
#define ___S_UTILS_HPP_____

#include <string>
#include <opencv2/core/core.hpp>
#include "targetplatform.h"

std::vector<std::string> GetFilenamesOfTypeInFolder(std::string folder_dir_name, std::string extension_of_file);


template <class T>
inline std::string to_istring(const T& t)
{
    std::stringstream ss;
    ss << static_cast<int>(t);
    return ss.str();
}

template <class T>
inline std::string to_sstring(const T& t)
{
    std::stringstream ss;
    ss << (t);
    return ss.str();
}

inline std::string char_to_string(const char & input)
{
    char tempstr[2];
    tempstr[0] = input;
    tempstr[1] = 0;
    return std::string(tempstr);
}


//returns the chars after the LAST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);

//returns the chars after the FIRST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
//leaves behind what was before the delim
std::string trim_chars_after_first_instance_of_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);

//returns what was before the delim
//leaves behind the chars after the FIRST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_first_instance_of_delim_return_first(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);


std::string get_extension_from_filename(const std::string & filename);
std::string eliminate_extension_from_filename(std::string & filename);



bool check_if_file_exists(const std::string & filename);
bool check_if_directory_exists(const std::string & dir_name);
int CountFilesOfTypeInFolder(std::string folder_dir_name, std::string desired_extension);




#endif // ___S_UTILS_HPP_____
