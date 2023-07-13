#pragma once

#include <stringapiset.h>
#include <map>
#include <regex>

namespace Dash
{
    class FStringUtility
    {
    public:
        /**
         * @brief Converts std::string into std::wstring.
         * @param str - will be converted into std::wstring.
         * @return Converted value as std::wstring.
         */
        static FORCEINLINE std::wstring UTF8ToWideString(const std::string& str)
        {
        /*
		    auto ret = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
		    if (!ret) {
			    throw std::system_error(GetLastError(), std::generic_category(), "failed to get the buffer size that is needed to store wstring");
		    }

		    std::wstring dst(ret, L'\0');
		    ret = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, dst.data(), static_cast<int>(dst.size()));
		    if (!ret) {
			    throw std::system_error(GetLastError(), std::generic_category(), "failed to translate from string to wstring");
		    }
        */
            std::wstring dst{str.begin(), str.end()};

		    return dst;
        }

        /**
	     * @brief Converts std::wstring into std::string.
	     * @param str - will be converted into std::string.
	     * @return Converted value as std::string.
	     */
	    static FORCEINLINE std::string WideStringToUTF8(const std::wstring& wstr)
	    {
		    auto ret = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		    if (!ret) {
			    throw std::system_error(GetLastError(), std::generic_category(), "failed to get the buffer size that is needed to store string");
		    }

		    std::string dst(ret, '\0');
		    ret = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, dst.data(), static_cast<int>(dst.size()), nullptr, nullptr);
		    if (!ret) {
			    throw std::system_error(GetLastError(), std::generic_category(), "failed to get the buffer size that is needed to store string");
		    }
		    return dst;
	    }

        /**
         * @brief Converts any datatype into std::string.
         *        Data type must support << operator.
         * @tparam T
         * @param value - will be converted into std::string.
         * @return Converted value as std::string.
         */
        template<typename T>
        static FORCEINLINE std::string ToString(T value)
        {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }

        /**
         * @brief Converts std::string into any datatype.
         *        Data type must support << operator.
         * @tparam T
         * @param str - std::string that will be converted into datatype T.
         * @return Variable of data type T.
         */
        template<typename T>
        static FORCEINLINE T ParseString(const std::string& str)
        {
            T result;
            std::istringstream(str) >> result;

            return result;
        }

        /**
         * @brief Converts std::string to lower case.
         * @param str - std::string that needs to be converted.
         * @return Lower case input std::string.
         */
        static FORCEINLINE std::string ToLower(const std::string& str)
        {
            auto result = str;
            std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) -> unsigned char
                {
                    return static_cast<unsigned char>(std::tolower(c));
                });

            return result;
        }

        /**
         * @brief Converts std::string to upper case.
         * @param str - std::string that needs to be converted.
         * @return Upper case input std::string.
         */
        static FORCEINLINE std::string ToUpper(const std::string& str)
        {
            auto result = str;
            std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) -> unsigned char
                {
                    return static_cast<unsigned char>(std::toupper(c));
                });

            return result;
        }

        /**
         * @brief Converts the first character of a string to uppercase letter and lowercases all other characters, if any.
         * @param str - input string to be capitalized.
         * @return A string with the first letter capitalized and all other characters lowercased. It doesn't modify the input string.
         */
        static FORCEINLINE std::string Capitalize(const std::string& str)
        {
            auto result = str;
            if (!result.empty())
            {
                result.front() = static_cast<char>(std::toupper(result.front()));
            }

            return result;
        }

        /**
         * @brief Converts only the first character of a string to uppercase letter, all other characters stay unchanged.
         * @param str - input string to be modified.
         * @return A string with the first letter capitalized. All other characters stay unchanged. It doesn't modify the input string.
         */
        static FORCEINLINE std::string CapitalizeFirstChar(const std::string& str)
        {
            auto result = ToLower(str);
            if (!result.empty())
            {
                result.front() = static_cast<char>(std::toupper(result.front()));
            }

            return result;
        }

        /**
         * @brief Checks if input std::string str contains specified substring.
         * @param str - std::string to be checked.
         * @param substring - searched substring.
         * @return True if substring was found in str, false otherwise.
         */
        static FORCEINLINE bool Contains(const std::string& str, const std::string& substring)
        {
            return str.find(substring) != std::string::npos;
        }

        /**
         * @brief Checks if input std::string str contains specified character.
         * @param str - std::string to be checked.
         * @param character - searched character.
         * @return True if character was found in str, false otherwise.
         */
        static FORCEINLINE bool Contains(const std::string& str, const char character)
        {
            return Contains(str, std::string(1, character));
        }

        /**
         * @brief Compares two std::strings ignoring their case (lower/upper).
         * @param str1 - std::string to compare
         * @param str2 - std::string to compare
         * @return True if str1 and str2 are equal, false otherwise.
         */
        static FORCEINLINE bool CompareIgnoreCase(const std::string& str1, const std::string& str2)
        {
            return ToLower(str1) == ToLower(str2);
        }

        /**
         * @brief Trims (in-place) white spaces from the left side of std::string.
         *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
         * @param str - input std::string to remove white spaces from.
         */
        static FORCEINLINE void TrimLeft(std::string& str)
        {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) { return !std::isspace(ch); }));
        }

        /**
         * @brief Trims (in-place) white spaces from the right side of std::string.
         *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
         * @param str - input std::string to remove white spaces from.
         */
        static FORCEINLINE void TrimRight(std::string& str)
        {
            str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) { return !std::isspace(ch); }).base(), str.end());
        }

        /**
         * @brief Trims (in-place) white spaces from the both sides of std::string.
         *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
         * @param str - input std::string to remove white spaces from.
         */
        static FORCEINLINE void Trim(std::string& str)
        {
            TrimLeft(str);
            TrimRight(str);
        }

        /**
         * @brief Trims white spaces from the left side of std::string.
         *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
         * @param str - input std::string to remove white spaces from.
         * @return Copy of input str with trimmed white spaces.
         */
        static FORCEINLINE std::string TrimLeftCopy(std::string str)
        {
            TrimLeft(str);
            return str;
        }

        /**
          * @brief Trims white spaces from the right side of std::string.
          *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
          * @param str - input std::string to remove white spaces from.
          * @return Copy of input str with trimmed white spaces.
          */
        static FORCEINLINE std::string TrimRightCopy(std::string str)
        {
            TrimRight(str);
            return str;
        }

        /**
          * @brief Trims white spaces from the both sides of std::string.
          *        Taken from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
          * @param str - input std::string to remove white spaces from.
          * @return Copy of input str with trimmed white spaces.
          */
        static FORCEINLINE std::string TrimCopy(std::string str)
        {
            Trim(str);
            return str;
        }

        /**
         * @brief Replaces (in-place) the first occurrence of target with replacement.
         *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
         * @param str - input std::string that will be modified.
         * @param target - substring that will be replaced with replacement.
         * @param replacement - substring that will replace target.
         * @return True if replacement was successful, false otherwise.
         */
        static FORCEINLINE bool ReplaceFirst(std::string& str, const std::string& target, const std::string& replacement)
        {
            const size_t start_pos = str.find(target);
            if (start_pos == std::string::npos)
            {
                return false;
            }

            str.replace(start_pos, target.length(), replacement);
            return true;
        }

        /**
         * @brief Replaces (in-place) last occurrence of target with replacement.
         *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
         * @param str - input std::string that will be modified.
         * @param target - substring that will be replaced with replacement.
         * @param replacement - substring that will replace target.
         * @return True if replacement was successful, false otherwise.
         */
        static FORCEINLINE bool ReplaceLast(std::string& str, const std::string& target, const std::string& replacement)
        {
            size_t start_pos = str.rfind(target);
            if (start_pos == std::string::npos)
            {
                return false;
            }

            str.replace(start_pos, target.length(), replacement);
            return true;
        }

        /**
         * @brief Replaces (in-place) all occurrence of target with replacement.
         *        Taken from: http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
         * @param str - input std::string that will be modified.
         * @param target - substring that will be replaced with replacement.
         * @param replacement - substring that will replace target.
         * @return True if replacement was successful, false otherwise.
         */
        static FORCEINLINE bool ReplaceAll(std::string& str, const std::string& target, const std::string& replacement)
        {
            if (target.empty())
            {
                return false;
            }

            size_t start_pos = 0;
            const bool found_substring = str.find(target, start_pos) != std::string::npos;

            while ((start_pos = str.find(target, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, target.length(), replacement);
                start_pos += replacement.length();
            }

            return found_substring;
        }

        /**
         * @brief Checks if std::string str ends with specified suffix.
         * @param str - input std::string that will be checked.
         * @param suffix - searched suffix in str.
         * @return True if suffix was found, false otherwise.
         */
        static FORCEINLINE bool EndsWith(const std::string& str, const std::string& suffix)
        {
            const auto suffix_start = str.size() - suffix.size();
            const auto result = str.find(suffix, suffix_start);
            return (result == suffix_start) && (result != std::string::npos);
        }

        /**
         * @brief Checks if std::string str ends with specified character.
         * @param str - input std::string that will be checked.
         * @param suffix - searched character in str.
         * @return True if ends with character, false otherwise.
         */
        static FORCEINLINE bool EndsWith(const std::string& str, const char suffix)
        {
            return !str.empty() && (str.back() == suffix);
        }

        /**
         * @brief Checks if std::string str starts with specified prefix.
         * @param str - input std::string that will be checked.
         * @param prefix - searched prefix in str.
         * @return True if prefix was found, false otherwise.
         */
        static FORCEINLINE bool StartsWith(const std::string& str, const std::string& prefix)
        {
            return str.rfind(prefix, 0) == 0;
        }

        /**
         * @brief Checks if std::string str starts with specified character.
         * @param str - input std::string that will be checked.
         * @param prefix - searched character in str.
         * @return True if starts with character, false otherwise.
         */
        static FORCEINLINE bool StartsWith(const std::string& str, const char prefix)
        {
            return !str.empty() && (str.front() == prefix);
        }

        /**
         * @brief Splits input std::string str according to input delim.
         * @param str - std::string that will be splitted.
         * @param delim - the delimiter.
         * @return std::vector<std::string> that contains all splitted tokens.
         */
        static FORCEINLINE std::vector<std::string> Split(const std::string& str, const char delim)
        {
            std::vector<std::string> tokens;
            std::stringstream ss(str);

            std::string token;
            while (std::getline(ss, token, delim))
            {
                tokens.push_back(token);
            }

            // Match semantics of split(str,str)
            if (str.empty() || EndsWith(str, delim)) {
                tokens.emplace_back();
            }

            return tokens;
        }

        /**
         * @brief Splits input std::string str according to input std::string delim.
         *        Taken from: https://stackoverflow.com/a/46931770/1892346.
         * @param str - std::string that will be split.
         * @param delim - the delimiter.
         * @return std::vector<std::string> that contains all splitted tokens.
         */
        static FORCEINLINE std::vector<std::string> Split(const std::string& str, const std::string& delim)
        {
            size_t pos_start = 0, pos_end, delim_len = delim.length();
            std::string token;
            std::vector<std::string> tokens;

            while ((pos_end = str.find(delim, pos_start)) != std::string::npos)
            {
                token = str.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                tokens.push_back(token);
            }

            tokens.push_back(str.substr(pos_start));
            return tokens;
        }

        /**
         * @brief Splits input string using regex as a delimiter.
         * @param src - std::string that will be split.
         * @param rgx_str - the set of delimiter characters.
         * @return vector of resulting tokens.
         */
        static FORCEINLINE std::vector<std::string> RegexSplit(const std::string& src, const std::string& rgx_str)
        {
            std::vector<std::string> elems;
            const std::regex rgx(rgx_str);
            std::sregex_token_iterator iter(src.begin(), src.end(), rgx, -1);
            std::sregex_token_iterator end;
            while (iter != end)
            {
                elems.push_back(*iter);
                ++iter;
            }
            return elems;
        }

        /**
         * @brief Splits input string using regex as a delimiter.
         * @param src - std::string that will be split.
         * @param dest - map of matched delimiter and those being split.
         * @param rgx_str - the set of delimiter characters.
         * @return True if the parsing is successfully done.
         */
        static FORCEINLINE std::map<std::string, std::string> RegexSplitMap(const std::string& src, const std::string& rgx_str)
        {
            std::map<std::string, std::string> dest;
            std::string tstr = src + " ";
            std::regex rgx(rgx_str);
            std::sregex_token_iterator niter(tstr.begin(), tstr.end(), rgx);
            std::sregex_token_iterator viter(tstr.begin(), tstr.end(), rgx, -1);
            std::sregex_token_iterator end;
            ++viter;
            while (niter != end)
            {
                dest[*niter] = *viter;
                ++niter;
                ++viter;
            }

            return dest;
        }

        /**
         * @brief Splits input string using any delimiter in the given set.
         * @param str - std::string that will be split.
         * @param delims - the set of delimiter characters.
         * @return vector of resulting tokens.
         */
        static FORCEINLINE std::vector<std::string> SplitAny(const std::string& str, const std::string& delims)
        {
            std::string token;
            std::vector<std::string> tokens;

            size_t pos_start = 0;
            for (size_t pos_end = 0; pos_end < str.length(); ++pos_end)
            {
                if (Contains(delims, str[pos_end]))
                {
                    token = str.substr(pos_start, pos_end - pos_start);
                    tokens.push_back(token);
                    pos_start = pos_end + 1;
                }
            }

            tokens.push_back(str.substr(pos_start));
            return tokens;
        }

        /**
         * @brief Joins all elements of std::vector tokens of arbitrary datatypes
         *        into one std::string with delimiter delim.
         * @tparam T - arbitrary data type.
         * @param tokens - vector of tokens.
         * @param delim - the delimiter.
         * @return std::string with joined elements of vector tokens with delimiter delim.
         */
        template<typename T>
        static FORCEINLINE std::string Join(const std::vector<T>& tokens, const std::string& delim)
        {
            std::ostringstream result;
            for (auto it = tokens.begin(); it != tokens.end(); ++it)
            {
                if (it != tokens.begin())
                {
                    result << delim;
                }

                result << *it;
            }

            return result.str();
        }

        /**
         * @brief In place removal of all empty strings in a vector<string>
         * @param tokens - vector of strings.
         */
        static FORCEINLINE void DropEmpty(std::vector<std::string>& tokens)
        {
            auto last = std::remove_if(tokens.begin(), tokens.end(), [](const std::string& s) { return s.empty(); });
            tokens.erase(last, tokens.end());
        }

        /**
         * @brief In place removal of all empty strings in a vector<string>
         * @param tokens - vector of strings.
         * @return vector of non-empty tokens.
         */
        static FORCEINLINE std::vector<std::string> DropEmptyCopy(std::vector<std::string> tokens)
        {
            DropEmpty(tokens);
            return tokens;
        }

        /**
         * @brief In place removal of all duplicate strings in a vector<string> where order is not to be maintained
         *        Taken from: C++ Primer V5
         * @param tokens - vector of strings.
         * @return vector of non-duplicate tokens.
         */
        static FORCEINLINE void DropDuplicate(std::vector<std::string>& tokens)
        {
            std::sort(tokens.begin(), tokens.end());
            auto end_unique = std::unique(tokens.begin(), tokens.end());
            tokens.erase(end_unique, tokens.end());
        }

        /**
         * @brief Removal of all duplicate strings in a vector<string> where order is not to be maintained
         *        Taken from: C++ Primer V5
         * @param tokens - vector of strings.
         * @return vector of non-duplicate tokens.
         */
        static FORCEINLINE std::vector<std::string> DropDuplicateCopy(std::vector<std::string> tokens)
        {
            std::sort(tokens.begin(), tokens.end());
            auto end_unique = std::unique(tokens.begin(), tokens.end());
            tokens.erase(end_unique, tokens.end());
            return tokens;
        }

        /**
         * @brief Creates new std::string with repeated n times substring str.
         * @param str - substring that needs to be repeated.
         * @param n - number of iterations.
         * @return std::string with repeated substring str.
         */
        static FORCEINLINE std::string Repeat(const std::string& str, unsigned n)
        {
            std::string result;

            for (unsigned i = 0; i < n; ++i)
            {
                result += str;
            }

            return result;
        }

        /**
         * @brief Creates new std::string with repeated n times char c.
         * @param c - char that needs to be repeated.
         * @param n - number of iterations.
         * @return std::string with repeated char c.
         */
        static FORCEINLINE std::string Repeat(char c, unsigned n)
        {
            return std::string(n, c);
        }

        /**
         * @brief Checks if input std::string str matches specified reular expression regex.
         * @param str - std::string to be checked.
         * @param regex - the std::regex regular expression.
         * @return True if regex matches str, false otherwise.
         */
        static FORCEINLINE bool Matches(const std::string& str, const std::regex& regex)
        {
            return std::regex_match(str, regex);
        }

        /**
         * @brief Sort input std::vector<std::string> strs in ascending order.
         * @param strs - std::vector<std::string> to be checked.
         */
        template<typename T>
        static FORCEINLINE void SortingAscending(std::vector<T>& strs)
        {
            std::sort(strs.begin(), strs.end());
        }

        /**
         * @brief Sorted input std::vector<std::string> strs in descending order.
         * @param strs - std::vector<std::string> to be checked.
         */
        template<typename T>
        static FORCEINLINE void SortingDescending(std::vector<T>& strs)
        {
            std::sort(strs.begin(), strs.end(), std::greater<T>());
        }

        /**
         * @brief Reverse input std::vector<std::string> strs.
         * @param strs - std::vector<std::string> to be checked.
         */
        template<typename T>
        static FORCEINLINE void ReverseInplace(std::vector<T>& strs)
        {
            std::reverse(strs.begin(), strs.end());
        }

        /**
         * @brief Reverse input std::vector<std::string> strs.
         * @param strs - std::vector<std::string> to be checked.
         */
        template<typename T>
        static FORCEINLINE std::vector<T> ReverseCopy(std::vector<T> strs)
        {
            std::reverse(strs.begin(), strs.end());
            return strs;
        }
    };
}