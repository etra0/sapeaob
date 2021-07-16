# SapeAOB - A compile time function generation for pattern scans.

A micro-library which aids to create pattern scanning functions to be created at compile time to
enjoy that sweet compiler optimizations when doing ifs.

# Usage
```cpp
std::vector<std::uint8_t> test_arr{0xFF, 0xAA, 0xDF, 0xCC};
// You can store the pattern to do the scan later. `sapeaob::ANY` corresponds
// to a wildcard in the pattern.
sapeaob::pattern<0xAA, sapeaob::ANY, 0xCC> p{};
  
// You can scan a vector
std::uintptr_t result = p.scan_match(test_arr.begin(), test_arr.size());
CHECK(result == reinterpret_it(test_arr.begin() + 1));

// Or an array, or anything as long as you can offset it!
std::uint8_t test_arr2[] = {0xCC, 0xFF, 0xAA, 0xEE, 0xCC};
result = p.scan_match(test_arr2, sizeof(test_arr2));
CHECK(result == reinterpret_cast<std::uintptr_t>(test_arr2 + 2));
```