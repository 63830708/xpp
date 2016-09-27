/*
 * foothold.cc
 *
 *  Created on: May 10, 2016
 *      Author: winklera
 */

#include <xpp/hyq/foothold.h>

namespace xpp {
namespace hyq {

static const int kDim2d = 2;

Foothold::Foothold ()
{
  p.setZero();
  leg = LF;
  id = kFixedByStart;
}

Foothold::Foothold(Eigen::Vector3d _pos, LegID _leg)
    : p(_pos), leg(_leg)
{
  id = kFixedByStart;
};

Foothold::Foothold(double x, double y, double z, LegID _leg)
    : p(x, y, z), leg(_leg)
{
  id = kFixedByStart;
};

Eigen::Vector2d Foothold::GetXy() const
{
  return p.topRows<kDim2d>();
}

void Foothold::SetXy(const Vector2d& xy)
{
  p.topRows<kDim2d>() = xy;
}

void Foothold::SetXy(const StdVecEigen2d& xy, VecFoothold& footholds)
{
  assert(footholds.size() == xy.size());

  for (uint i=0; i<footholds.size(); ++i)
    footholds.at(i).SetXy(xy.at(i));
}

bool Foothold::IsInFootholds(LegID leg, const VecFoothold& footholds)
{
  return std::find_if(footholds.begin(),
                      footholds.end(),
                      [leg](const Foothold& f) { return f.leg == leg; }) != footholds.end();
}

int Foothold::GetLastIndex(LegID leg, const VecFoothold& footholds)
{
  auto it = std::find_if( footholds.rbegin(),
                          footholds.rend(),
                          [leg](const Foothold& f) { return f.leg == leg; });

  assert(it != footholds.rend()); // GetLastIndex: Leg does not exist in foothold
  return std::distance(footholds.begin(), it.base()-1);
}

Foothold Foothold::GetLastFoothold(LegID leg, const VecFoothold& footholds)
{
  int idx = GetLastIndex(leg, footholds);
  return footholds.at(idx);
}

void Foothold::UpdateFoothold(const Foothold& f_new, VecFoothold& footholds)
{
  if (IsInFootholds(f_new.leg, footholds)) {
    int idx = GetLastIndex(f_new.leg, footholds);
    footholds.at(idx) = f_new; // replace
  } else
    footholds.push_back(f_new); //append
}

bool Foothold::operator==(const Foothold& rhs) const
{
  return (p==rhs.p) && (leg==rhs.leg);
}

bool Foothold::operator!=(const Foothold& rhs) const
{
  return !(*this==rhs);
}


} // namespace hyq
} // namespace xpp
