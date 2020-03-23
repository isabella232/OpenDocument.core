#ifndef ODR_TABLEPOSITION_H
#define ODR_TABLEPOSITION_H

#include <cstdint>
#include <string>

namespace odr {

class TablePosition {
public:
  static std::uint32_t to_col_num(const std::string &);
  static std::string to_col_string(std::uint32_t);

  TablePosition() noexcept;
  TablePosition(std::uint32_t row, std::uint32_t col) noexcept;
  explicit TablePosition(const std::string &);

  std::uint32_t getRow() const noexcept { return row; }
  std::uint32_t getCol() const noexcept { return col; }
  std::string toString() const noexcept;

private:
  std::uint32_t row;
  std::uint32_t col;
};

} // namespace odr

#endif // ODR_TABLEPOSITION_H