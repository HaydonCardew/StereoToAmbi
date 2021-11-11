/*
*  This file is part of StereoToAmbi.
*
*  StereoToAmbi is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  StereoToAmbi is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with StereoToAmbi.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Tools.h"
#include <sstream>
#include <iostream>
#include <iomanip>

uint64_t Tools::factorial(int x)
{
    uint64_t val = x--;
    while (x > 1)
    {
        val *= x--;
    }
    return val;
}

uint64_t Tools::nCombinations(int n, int k)
{
    return factorial(n) / (factorial(n - k) * factorial(k));
}

std::string Tools::floatToString(float number, unsigned decimalPlaces)
{
    stringstream stream;
    stream << fixed << setprecision(decimalPlaces) << number;
    return stream.str();
}
