#ifndef RUNTIME_FUNC_HPP
#define RUNTIME_FUNC_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <sstream>

bool isNumber(const std::string &str)
{
    std::istringstream iss(str);
    double d;
    char c;
    return iss >> d && !(iss >> c);
}

template <typename T>
class SetLangVar
{
private:
    std::vector<T> data;

public:
    // Constractor and Destructor
    SetLangVar() : data() {}
    ~SetLangVar() {}

    // Getters
    std::vector<T> getData() const
    {
        return data;
    }

    int getSize() const
    {
        return data.size();
    }

    // Class Functions

    void clear()
    {
        data.clear();
    }

    void addValue(const T &value)
    {
        if (std::find(data.begin(), data.end(), value) != data.end())
        {
            return;
        }
        data.push_back(value);
    }

    void removeValue(const T &value)
    {
        auto it = std::find(data.begin(), data.end(), value);
        if (it != data.end())
        {
            data.erase(it);
        }
    }

    void addValueFromVar(const SetLangVar<T> &var)
    {
        for (const T &value : var.data)
        {
            addValue(value);
        }
    }

    void removeValueFromVar(const SetLangVar<T> &var)
    {
        for (const T &value : var.data)
        {
            removeValue(value);
        }
    }

    std::vector<T> intersectWithVar(const SetLangVar<T> &other) const
    {
        std::vector<T> intersection;
        for (int i = 0; i < data.size(); ++i)
        {
            for (int j = 0; j < other.data.size(); ++j)
            {
                if (data[i] == other.data[j])
                {
                    intersection.push_back(data[i]);
                    break;
                }
            }
        }
        return intersection;
    }

    void insertFromString(const std::string &str)
    {
        if (str.front() == '[' && str.back() == ']')
        {
            std::string trimmedStr = str.substr(1, str.length() - 2);

            std::string token;
            std::stringstream ss(trimmedStr);
            while (std::getline(ss, token, ','))
            {
                token.erase(0, token.find_first_not_of(' '));
                token.erase(token.find_last_not_of(' ') + 1);

                if (isNumber(token))
                {
                    std::stringstream valueStream(token);
                    T value;
                    valueStream >> value;
                    addValue(value);
                }
                else
                {
                    std::cout << "Invalid number: " << token << std::endl;
                    exit(1);
                }
            }
        }
    }

    void removeFromString(const std::string &str)
    {
        if (str.front() == '[' && str.back() == ']')
        {
            std::string trimmedStr = str.substr(1, str.length() - 2);

            std::string token;
            std::stringstream ss(trimmedStr);
            while (std::getline(ss, token, ','))
            {
                token.erase(0, token.find_first_not_of(' '));
                token.erase(token.find_last_not_of(' ') + 1);

                if (isNumber(token))
                {
                    std::stringstream valueStream(token);
                    T value;
                    valueStream >> value;
                    removeValue(value);
                }
                else
                {
                    std::cout << "Invalid number: " << token << std::endl;
                    exit(1);
                }
            }
        }
    }

    // Class Operators Overloading
    SetLangVar<T> &operator=(const SetLangVar<T> &other)
    {
        if (this != &other)
        {
            data = other.data;
        }
        return *this;
    }

    SetLangVar<T> &operator=(const std::string &str)
    {
        this->clear();
        this->insertFromString(str);
        return *this;
    }

    bool operator==(const SetLangVar<T> &var) const
    {
        auto dataCopy1 = data;
        auto dataCopy2 = var.data;
        std::sort(dataCopy1.begin(), dataCopy1.end());
        std::sort(dataCopy2.begin(), dataCopy2.end());
        return dataCopy1 == dataCopy2;
    }

    bool operator==(const std::string &str) const
    {
        SetLangVar<T> temp;
        temp.insertFromString(str);

        auto dataCopy1 = this->data;
        auto dataCopy2 = temp.getData();

        std::sort(dataCopy1.begin(), dataCopy1.end());
        std::sort(dataCopy2.begin(), dataCopy2.end());

        return dataCopy1 == dataCopy2;
    }

    SetLangVar<T> operator+(const SetLangVar<T> &other) const
    {
        SetLangVar<T> result(*this);
        result.addValueFromVar(other);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<!std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator+(const U &value) const
    {
        SetLangVar<T> result(*this);
        result.addValue(value);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator+(const U &value) const
    {
        SetLangVar<T> result(*this);
        if (value.front() == '{' && value.back() == '}')
        {
            result.insertFromString(value);
        }
        else
        {
            result.addValue(value);
        }
        return result;
    }

    SetLangVar<T> operator-(const SetLangVar<T> &other) const
    {
        SetLangVar<T> result(*this);
        result.removeValueFromVar(other);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<!std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator-(const U &value) const
    {
        SetLangVar<T> result(*this);
        result.removeValue(value);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator-(const U &value) const
    {
        SetLangVar<T> result(*this);
        if (value.front() == '{' && value.back() == '}')
        {
            result.removeFromString(value);
        }
        else
        {
            result.removeValue(value);
        }
        return result;
    }

    SetLangVar<T> operator*(const SetLangVar<T> &other) const
    {
        SetLangVar<T> result;
        result.data = this->intersectWithVar(other);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator*(const std::string &str) const
    {
        if (str.front() != '{' || str.back() != '}')
        {
            std::cerr << "Error: The input string must start with '{' and end with '}'." << std::endl;
            exit(1);
        }

        SetLangVar<T> temp;
        temp.insertFromString(str);
        SetLangVar<T> result;
        result.data = this->intersectWithVar(temp);
        return result;
    }

    template <typename U = T>
    typename std::enable_if<!std::is_same<U, std::string>::value, SetLangVar<T>>::type
    operator*(const std::string &str) const
    {
        if (str.front() != '[' || str.back() != ']')
        {
            std::cerr << "Error: The input string must start with '[' and end with ']'." << std::endl;
            exit(1);
        }

        SetLangVar<T> temp;
        temp.insertFromString(str);
        SetLangVar<T> result;
        result.data = this->intersectWithVar(temp);
        return result;
    }

    T &operator[](size_t i)
    {
        if (i >= data.size())
        {
            throw std::out_of_range("Index out of range");
        }
        return data[i];
    }

    const T &operator[](size_t i) const
    {
        if (i >= data.size())
        {
            throw std::out_of_range("Index out of range");
        }
        return data[i];
    }

    bool operator!() const
    {
        return data.empty();
    }

    template <typename U>
    friend typename std::enable_if<!std::is_same<U, std::string>::value, std::ostream &>::type
    operator<<(std::ostream &os, const SetLangVar<U> &var);

    template <typename U>
    friend typename std::enable_if<std::is_same<U, std::string>::value, std::ostream &>::type
    operator<<(std::ostream &os, const SetLangVar<U> &var);
};

// Overloaded output operator for SetLangVar
template <typename U>
typename std::enable_if<!std::is_same<U, std::string>::value, std::ostream &>::type
operator<<(std::ostream &os, const SetLangVar<U> &var)
{
    os << "[ ";
    for (size_t i = 0; i < var.data.size(); ++i)
    {
        os << var.data[i];
        if (i < var.data.size() - 1)
        {
            os << ", ";
        }
    }
    os << " ]";
    return os;
}

template <typename U>
typename std::enable_if<std::is_same<U, std::string>::value, std::ostream &>::type
operator<<(std::ostream &os, const SetLangVar<U> &var)
{
    os << "{ ";
    for (size_t i = 0; i < var.data.size(); ++i)
    {
        os << var.data[i]; 
        if (i < var.data.size() - 1)
        {
            os << ", ";
        }
    }
    os << " }";
    return os;
}

// Specialization Functions for when T is std::string
template <>
void SetLangVar<std::string>::insertFromString(const std::string &str)
{
    if (str.front() == '{' && str.back() == '}')
    {
        std::string trimmedStr = str.substr(1, str.length() - 2);
        std::string token;
        std::stringstream ss(trimmedStr);

        while (std::getline(ss, token, ','))
        {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            if (token.front() == '"' && token.back() == '"')
            {
                token = token.substr(1, token.length() - 2);
            }
            addValue(token);
        }
    }
    else
    {
        std::cout << "Invalid format: Missing enclosing braces { }" << std::endl;
        exit(1);
    }
}

template <>
void SetLangVar<std::string>::removeFromString(const std::string &str)
{
    if (str.front() == '{' && str.back() == '}')
    {
        std::string trimmedStr = str.substr(1, str.length() - 2);
        std::string token;
        std::stringstream ss(trimmedStr);
        while (std::getline(ss, token, ','))
        {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);

            if (token.front() == '"' && token.back() == '"')
            {
                std::string value = token.substr(1, token.length() - 2);
                removeValue(value);
            }
            else
            {
                std::cout << "Invalid string format: " << token << std::endl;
                exit(1);
            }
        }
    }
    else
    {
        std::cout << "Invalid format: Missing enclosing braces { }" << std::endl;
        exit(1);
    }
}

#endif // RUNTIME_FUNC_HPP
