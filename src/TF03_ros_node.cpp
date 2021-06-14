#include <TF03.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tf03_ros_node");
  ros::NodeHandle nh("~");
  std::string id = "TF03";
  std::string portName;
  int baud_rate;
  int polling_rate;
  benewake::TF03 *tf03_obj;

  nh.param("serial_port", portName, std::string("/dev/ttyAMA2"));
  nh.param("baud_rate", baud_rate, 115200);

  tf03_obj = new benewake::TF03(portName, baud_rate);
  // ros::Publisher pub_range = nh.advertise<sensor_msgs::Range>(id, 1000, true);
  ros::Publisher pub_range = nh.advertise<sensor_msgs::Range>(id, 1);
  sensor_msgs::Range TF03_range;
  TF03_range.radiation_type = sensor_msgs::Range::INFRARED;
  TF03_range.field_of_view = 0.00872665; // 0.5 degree radians
  TF03_range.min_range = 0.1;
  TF03_range.max_range = 180;
  TF03_range.header.frame_id = id;
  float dist = 0;
  ROS_INFO_STREAM("Start processing ...");

  while(ros::master::check() && ros::ok())
  {
    ros::spinOnce();
    dist = tf03_obj->getDist();
    if(dist > 0 && dist < TF03_range.max_range)
    {
      TF03_range.range = dist;
      TF03_range.header.stamp = ros::Time::now();
      pub_range.publish(TF03_range);
    }
    else if(dist == -1.0)
    {
      ROS_ERROR_STREAM("Failed to read data. TF03 ros node stopped!");
      break;
    }
    else if(dist == 0.0)
    {
      ROS_ERROR_STREAM("Data validation error!");
    }
  }
  tf03_obj->closePort();
}
