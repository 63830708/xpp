/**
 @file    marker_array_builder.cc
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    May 31, 2016
 @brief   Defines a class that builds rviz markers
 */

#include <xpp/marker_array_builder.h>
#include <xpp/ros/ros_helpers.h>

namespace xpp {

static const std::string supp_tr_topic = "support_polygons";
static const std::string frame_id_ = "world";

MarkerArrayBuilder::MarkerArrayBuilder()
{
}

void
MarkerArrayBuilder::AddStart (MarkerArray& msg) const
{
  auto start = robot_traj_.front().GetBase().lin.Get2D().p;
  AddPoint(msg, start, "start", visualization_msgs::Marker::CYLINDER);
}

void
MarkerArrayBuilder::AddStartStance (MarkerArray& msg) const
{
  AddFootholds(msg, robot_traj_.front().GetContacts(), "start_stance", visualization_msgs::Marker::CUBE, 1.0);
}

void
MarkerArrayBuilder::AddSupportPolygons (MarkerArray& msg) const
{
  int prev_phase = -1;
  for (const auto& state : robot_traj_) {

    if (state.GetCurrentPhase() != prev_phase) {

      // plot in color of last swingleg
      EEID swingleg = EEID::E0;
      for (auto ee : state.GetEndeffectors())
        if (!state.GetContactState().At(ee))
          swingleg = ee;

      BuildSupportPolygon(msg, state.GetContacts(), swingleg);
      prev_phase = state.GetCurrentPhase();
    }
  }

  // delete the other markers, maximum of 30 support polygons.
  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;
  for (uint j=prev_phase; j<30; ++j) {
    visualization_msgs::Marker marker;
    marker.id = i++;
    marker.ns = supp_tr_topic;
    marker.action = visualization_msgs::Marker::DELETE;
    msg.markers.push_back(marker);
  }
}

void
MarkerArrayBuilder::AddFootholds (MarkerArray& msg) const
{
  int prev_phase = -1;
  ContactVec contacts;
  for (const auto& state : robot_traj_)
    if (state.GetCurrentPhase() != prev_phase) {
      for (auto c : state.GetContacts())
        contacts.push_back(c);
      prev_phase = state.GetCurrentPhase();
    }

  AddFootholds(msg, contacts, "footholds", visualization_msgs::Marker::SPHERE, 1.0);
}

void
MarkerArrayBuilder::BuildSupportPolygon(
    visualization_msgs::MarkerArray& msg,
    const ContactVec& stance,
    EEID leg_id) const
{
//  static int i=0;
//  geometry_msgs::PolygonStamped polygon_msg;
//  polygon_msg.header.frame_id = "world";
//  polygon_msg.header.seq = i++;

  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;

  visualization_msgs::Marker marker;
  marker.id = i;
  marker.header.frame_id = frame_id_;
  marker.header.stamp = ::ros::Time();
  marker.ns = supp_tr_topic; //"leg " + std::to_string(leg_id);
  marker.action = visualization_msgs::Marker::MODIFY;
 //    marker.lifetime = ros::Duration(10);
  marker.scale.x = marker.scale.y = marker.scale.z = 1.0;
  marker.color = GetLegColor(leg_id);
  marker.color.a = 0.15;

  static const int points_per_triangle =3;
  if (stance.size() == points_per_triangle) {
    marker.type = visualization_msgs::Marker::TRIANGLE_LIST;
    for (size_t i=0; i<points_per_triangle; ++i) {
      geometry_msgs::Point point;
      point.x = stance.at(i).p.x();
      point.y = stance.at(i).p.y();
      point.z = stance.at(i).p.z();
      marker.points.push_back(point);
    }
    msg.markers.push_back(marker);
  }

  // if only two contact points exist, draw line
  static const int points_per_line = 2;
  if (stance.size() == points_per_line) {
    marker.type = visualization_msgs::Marker::LINE_STRIP;
    marker.scale.x = 0.02;

    for (size_t i=0; i<points_per_line; ++i) {
      geometry_msgs::Point point;
      point.x = stance.at(i).p.x();
      point.y = stance.at(i).p.y();
      point.z = stance.at(i).p.z();
      marker.points.push_back(point);
    }
    msg.markers.push_back(marker);
  }
}

void MarkerArrayBuilder::AddPoint(
    visualization_msgs::MarkerArray& msg,
    const Eigen::Vector2d& goal,
    std::string rviz_namespace,
    int marker_type) const
{
  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;

  Marker marker;
  marker.id = i;
  marker = GenerateMarker(goal, marker_type, 0.02);
  marker.ns = rviz_namespace;
  marker.scale.z = 0.04;
  marker.color.a = 1.0;
  marker.color.r = 0.0;
  marker.color.g = 0.0;
  marker.color.b = 0.0;
  msg.markers.push_back(marker);
}

visualization_msgs::Marker
MarkerArrayBuilder::GenerateMarker(Eigen::Vector2d pos, int32_t type, double size) const
{
  visualization_msgs::Marker marker;
  marker.pose.position.x = pos.x();
  marker.pose.position.y = pos.y();
  marker.pose.position.z = 0.0;
  marker.header.frame_id = frame_id_;
  marker.header.stamp = ::ros::Time();
  marker.type = type;
  marker.action = visualization_msgs::Marker::MODIFY;
//  marker.lifetime = ::ros::Duration(0.01);
  marker.scale.x = marker.scale.y = marker.scale.z = size;
  marker.color.a = 1.0;

  return marker;
}

void
MarkerArrayBuilder::AddLineStrip(visualization_msgs::MarkerArray& msg,
                           double center_x, double depth_x,
                           const std::string& rviz_namespace) const
{
  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;

  visualization_msgs::Marker line_strip;
  line_strip.header.frame_id = frame_id_;
  line_strip.header.stamp = ::ros::Time::now();
  line_strip.id = i;
  line_strip.type = visualization_msgs::Marker::LINE_STRIP;
  line_strip.ns = rviz_namespace;
  line_strip.action = visualization_msgs::Marker::MODIFY;
  line_strip.pose.orientation.x = line_strip.pose.orientation.y = line_strip.pose.orientation.z = 0.0;
  line_strip.pose.orientation.w = 1.0;
  line_strip.color.b = 1.0;
  line_strip.color.a = 0.2;
  line_strip.scale.x = depth_x;
  geometry_msgs::Point p1, p2;
  p1.x = center_x;
  p1.y = -0.5;
  p1.z = 0;

  p2 = p1;
  p2.y = -p1.y;

  line_strip.points.push_back(p1);
  line_strip.points.push_back(p2);
  msg.markers.push_back(line_strip);
}

void
MarkerArrayBuilder::AddEllipse(visualization_msgs::MarkerArray& msg,
                               double center_x, double center_y,
                               double width_x, double width_y,
                               const std::string& rviz_namespace) const
{

  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;

  visualization_msgs::Marker ellipse;
  ellipse.header.frame_id = frame_id_;
  ellipse.header.stamp = ::ros::Time::now();
  ellipse.id = i;
  ellipse.type = visualization_msgs::Marker::CYLINDER;
  ellipse.ns = rviz_namespace;
  ellipse.action = visualization_msgs::Marker::MODIFY;
  ellipse.pose.position.x = center_x;
  ellipse.pose.position.y = center_y;
  ellipse.pose.orientation.x = ellipse.pose.orientation.y = ellipse.pose.orientation.z = 0.0;
  ellipse.pose.orientation.w = 1.0;
  ellipse.color.b = 1.0;
  ellipse.color.a = 0.2;

  ellipse.scale.x = width_x;
  ellipse.scale.y = width_y;
  ellipse.scale.z = 0.01; // height of cylinder

  msg.markers.push_back(ellipse);
}


void
MarkerArrayBuilder::AddBodyTrajectory (MarkerArray& msg) const
{
  double dt = 0.01;
  double marker_size = 0.011;
  AddTrajectory(msg, "body", dt, marker_size,
                [](const StateLin3d& base){return base.Get2D().p;}
  );
}

void
MarkerArrayBuilder::AddZmpTrajectory (MarkerArray& msg) const
{
  double dt = 0.1;
  double marker_size = 0.011;
  AddTrajectory(msg, "zmp", dt, marker_size,
                [](const StateLin3d& base)
                {return base.GetZmp(base.p.z());}
  );
}

void
MarkerArrayBuilder::AddTrajectory(visualization_msgs::MarkerArray& msg,
                                  const std::string& rviz_namespace,
                                  double dt,
                                  double marker_size,
                                  const FctPtr& Get2dValue) const
{
  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;

  double T = robot_traj_.back().GetTime() - robot_traj_.front().GetTime();
  double traj_dt = T/robot_traj_.size();

  for (double t(0.0); t < T; t+= dt) {
    visualization_msgs::Marker marker;
    auto state = robot_traj_.at(floor(t/traj_dt));

    marker = GenerateMarker(Get2dValue(state.GetBase().lin),
                            visualization_msgs::Marker::SPHERE,
                            marker_size);

    marker.id = i++;
    marker.ns = rviz_namespace;

    // plot in color of last swingleg
    marker.color.r = marker.color.g = marker.color.b = 0.5; // no swingleg
    marker.color.a = 1.0;
    for (auto ee : state.GetEndeffectors())
      if (!state.GetContactState().At(ee))
        marker.color = GetLegColor(ee);

    msg.markers.push_back(marker);
  }

  // delete the other markers
  for (double t=T; t < 10.0; t+= dt)
  {
    visualization_msgs::Marker marker;

    marker.id = i++;
    marker.ns = rviz_namespace;
    marker.action = visualization_msgs::Marker::DELETE;
    msg.markers.push_back(marker);
  }
}

void MarkerArrayBuilder::AddFootholds(
    visualization_msgs::MarkerArray& msg,
    const ContactVec& contacts,
    const std::string& rviz_namespace,
    int32_t type,
    double alpha) const
{

  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;
  for (uint j=0; j<contacts.size(); ++j) {

    visualization_msgs::Marker marker_msg;
    marker_msg.type = type;
    marker_msg.action = visualization_msgs::Marker::MODIFY;
    marker_msg.pose.position = ros::RosHelpers::XppToRos<geometry_msgs::Point>(contacts.at(j).p);
    marker_msg.header.frame_id = frame_id_;
    marker_msg.header.stamp = ::ros::Time();
    marker_msg.ns = rviz_namespace;
    marker_msg.id = i++;
//    marker_msg.lifetime = ros::Duration(10);
    marker_msg.scale.x = 0.04;
    marker_msg.scale.y = 0.04;
    marker_msg.scale.z = 0.04;
    marker_msg.color = GetLegColor(contacts.at(j).ee);
    marker_msg.color.a = alpha;

    msg.markers.push_back(marker_msg);
  }

  // maximum of 30 steps
  for (int k=contacts.size(); k < 80; k++)
  {
    visualization_msgs::Marker marker;

    marker.id = i++;
    marker.ns = rviz_namespace;
    marker.action = visualization_msgs::Marker::DELETE;
    msg.markers.push_back(marker);
  }
}

std_msgs::ColorRGBA MarkerArrayBuilder::GetLegColor(EEID ee) const
{
  // define a few colors
  std_msgs::ColorRGBA red, green, blue, white, brown, yellow, purple;
  red.a = green.a = blue.a = white.a = brown.a = yellow.a = purple.a = 1.0;

  red.r = 1.0; red.g = 0.0; red.b = 0.0;
  green.r = 0.0; green.g = 150./255; green.b = 76./255;
  blue.r = 0.0; blue.g = 102./255; blue.b = 204./255;
  brown.r = 122./255; brown.g = 61./255; brown.b = 0.0;
  white.b = white.g = white.r = 1.0;
  yellow.r = 204./255; yellow.g = 204./255; yellow.b = 0.0;
//  purple.r = 123./255; purple.g = 104./255; purple.b = 238./255;
  purple.r = 72./255; purple.g = 61./255; purple.b = 139./255;


  std_msgs::ColorRGBA color_leg;
  switch (ee) {
    case EEID::E1:
      color_leg = purple;
      break;
    case EEID::E3:
      color_leg = green;
      break;
    case EEID::E0:
      color_leg = blue;
      break;
    case EEID::E2:
      color_leg = brown;
      break;
    default:
      break;
  }

  return color_leg;
}

//void
//MarkerArrayBuilder::AddPendulum(visualization_msgs::MarkerArray& msg,
//                                const ComMotion& com_motion,
//                                const MotionStructure& motion_structure,
//                                double walking_height,
//                                const std::string& rviz_namespace,
//                                double alpha) const
//{
//
//  int i = (msg.markers.size() == 0)? 0 : msg.markers.back().id + 1;
//
//  visualization_msgs::Marker marker;
//  marker.header.frame_id = frame_id_;
//  marker.header.stamp = ::ros::Time::now();
//  marker.action = visualization_msgs::Marker::MODIFY;
//  marker.type = visualization_msgs::Marker::LINE_STRIP;
//  marker.scale.x = 0.01; // thinkness of pendulum pole
//  marker.color.a = alpha;
//
//  // everything sent here will be overwritten
//  marker.ns = rviz_namespace;
//  marker.id = 0;
//
//  double dt = 0.01;
//  for (double t(0.0); t < com_motion.GetTotalTime(); t+= dt)
//  {
//    marker.points.clear();
//    xpp::utils::StateLin2d cog_state = com_motion.GetCom(t);
//    geometry_msgs::Point point;
//    point.x = cog_state.p.x();
//    point.y = cog_state.p.y();
//    point.z = walking_height;
//    marker.points.push_back(point);
//
//    Eigen::Vector2d zmp = xpp::opt::ZeroMomentPoint::CalcZmp(cog_state.Make3D(), walking_height);
//    point.x = zmp.x();
//    point.y = zmp.y();
//    point.z = 0.0;
//    marker.points.push_back(point);
//
//
//    auto phase = motion_structure.GetCurrentPhase(t);
//    if ( !phase.IsStep() ) {
//      marker.color.r = marker.color.g = marker.color.b = 0.1;
//    } else {
//      // take color of first swingleg
//      auto swing_leg = phase.swinglegs_.front().ee;
//      marker.color = GetLegColor(swing_leg);
//
//    }
//
//    msg.markers.push_back(marker);
//  }
//
////  // delete the other markers
////  for (double t=com_motion.GetTotalTime(); t < 10.0; t+= dt)
////  {
////    visualization_msgs::Marker marker;
////
////    marker.id = i++;
////    marker.ns = rviz_namespace;
////    marker.action = visualization_msgs::Marker::DELETE;
////    msg.markers.push_back(marker);
////  }
//}

} /* namespace xpp */
