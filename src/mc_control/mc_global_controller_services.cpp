#include <mc_control/mc_global_controller.h>

#include <RBDyn/FK.h>
#include <RBDyn/FV.h>

namespace mc_control
{

/* Called by the RT component to access actual Controllers service */
bool MCGlobalController::joint_up(const std::string & jname)
{
  if(controller)
  {
    return controller->joint_up(jname);
  }
  else
  {
    return false;
  }
}
bool MCGlobalController::joint_down(const std::string & jname)
{
  if(controller)
  {
    return controller->joint_down(jname);
  }
  else
  {
    return false;
  }
}
bool MCGlobalController::set_joint_pos(const std::string & jname, const double & pos)
{
  if(controller)
  {
    return controller->set_joint_pos(jname, pos);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::get_joint_pos(const std::string & jname, double & v)
{
  if(controller)
  {
    if(controller->robot().hasJoint(jname))
    {
      const auto & q = controller->robot().mbc().q[controller->robot().jointIndexByName(jname)];
      if(q.size() == 1)
      {
        v = q[0];
        return true;
      }
    }
  }
  return false;
}

bool MCGlobalController::change_ef(const std::string & ef_name)
{
  if(current_ctrl == BODY6D)
  {
    return body6d_controller->change_ef(ef_name);
  }
  else
  {
    return false;
  }
}
bool MCGlobalController::translate_ef(const Eigen::Vector3d & t)
{
  if(current_ctrl == BODY6D)
  {
    return body6d_controller->translate_ef(t);
  }
  else if(current_ctrl == EGRESS)
  {
    return egress_controller->move_ef(t, Eigen::Matrix3d::Identity());
  }
  else
  {
    return false;
  }
}
bool MCGlobalController::rotate_ef(const Eigen::Matrix3d & m)
{
  if(current_ctrl == BODY6D)
  {
    return body6d_controller->rotate_ef(m);
  }
  else if(current_ctrl == EGRESS)
  {
    return egress_controller->move_ef(Eigen::Vector3d(0,0,0), m);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::move_com(const Eigen::Vector3d & v)
{
  if(current_ctrl == COM)
  {
    return com_controller->move_com(v);
  }
  else if(current_ctrl == EGRESS)
  {
    return egress_controller->move_com(v);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::play_next_stance()
{
  if(current_ctrl == SEQ)
  {
    return seq_controller->play_next_stance();
  }
  else if(current_ctrl == EGRESS)
  {
    return egress_controller->next_phase();
  }
  else if(current_ctrl == EGRESS_MRQP)
  {
    egress_mrqp_controller->nextPhase();
    return true;
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::change_wheel_angle(double theta)
{
  if(current_ctrl == DRIVING)
  {
    return driving_controller->changeWheelAngle(theta);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::change_ankle_angle(double theta)
{
  if(current_ctrl == DRIVING)
  {
    return driving_controller->changeAnkleAngle(theta);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::change_gaze(double pan, double tilt)
{
  if(current_ctrl == DRIVING)
  {
    return driving_controller->changeGaze(pan, tilt);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::change_wrist_angle(double yaw)
{
  if(current_ctrl == DRIVING)
  {
    return driving_controller->changeWristAngle(yaw);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::driving_service(double w, double a, double p, double t)
{
  if(current_ctrl == DRIVING)
  {
    return driving_controller->driving_service(w, a, p, t);
  }
  else
  {
    return false;
  }
}

bool MCGlobalController::send_msg(const std::string & msg)
{
  if(controller)
  {
    return controller->read_msg(const_cast<std::string&>(msg));
  }
  return false;
}

bool MCGlobalController::send_recv_msg(const std::string & msg, std::string & out)
{
  if(controller)
  {
    return controller->read_write_msg(const_cast<std::string&>(msg), out);
  }
  return false;
}

}