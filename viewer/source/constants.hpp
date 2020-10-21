#include <QtMath>

namespace cnst
{
const float pi   = qAcos (-1);
const float pi_2 = pi / 2.0f;

constexpr float max_ui_float_decimal_points = 6.0f;
const float     min_ui_float = std::pow (10.0f, -max_ui_float_decimal_points);

constexpr float screen_in_pixels_2d = 512.0f;
constexpr float zoom_factor         = 5.0f;
} // namespace cnst