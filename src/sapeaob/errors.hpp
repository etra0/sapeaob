#include <exception>

namespace sapeaob {

struct pattern_not_found : public std::exception {
  const char *what() const throw() { return "Pattern couldn't be found"; }
};

} // namespace sapeaob
