#include <exception>

namespace sapeaob {

struct pattern_not_found : public std::exception {
  const char *what() const throw() { return "Pattern couldn't be found"; }
};

struct size_error : public std::exception {
  const char *what() const throw() {
    return "The type size doesn't match with the number of bytes";
  }
};

} // namespace sapeaob
