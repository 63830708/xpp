/**
 @file    a_inverse_kinematics.h
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Jun 6, 2016
 @brief   Declares an interface to represent any inverse dynamics class.
 */

#ifndef USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_OPT_A_INVERSE_KINEMATICS_H_
#define USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_OPT_A_INVERSE_KINEMATICS_H_

#include <Eigen/Dense>

#include <xpp/endeffectors.h>
#include "joints.h"

namespace xpp {

/** @brief An abstract class that every inverse dynamics class must conform with.
  *
  * This class is responsible for calculating the joint angles of a robot given
  * an endeffector position (=inverse kinematics).
  */

class AInverseKinematics {
public:
  using Joints1EE   = Eigen::VectorXd;
  using Vector3d    = Eigen::Vector3d;
  using EEPosition  = Vector3d;
  using EEID        = EndeffectorID;

  AInverseKinematics () {};
  virtual ~AInverseKinematics () {};

  /** @brief Calculates the joint angles to reach for a specific endeffector position
    *
    * @param pos_b the 3d-position of the endeffector expressed in the base frame.
    * @param ee the number of endeffector that the above position is referring to.
    * @return the joints angles of the robot.
    */
  virtual Joints GetAllJointAngles(const EndeffectorsPos& pos_b) const = 0;
};

} /* namespace xpp */

#endif /* USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_OPT_A_INVERSE_KINEMATICS_H_ */
