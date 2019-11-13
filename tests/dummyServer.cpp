/*
 * Copyright 2015-2019 CNRS-UM LIRMM, CNRS-AIST JRL
 */

#include <mc_control/ControllerServer.h>

#include <mc_rtc/gui.h>

#include <chrono>
#include <thread>

#ifndef M_PI
#  include <boost/math/constants/constants.hpp>
#  define M_PI boost::math::constants::pi<double>()
#endif

struct DummyProvider
{
  double value = 42.0;
  Eigen::Vector3d point = Eigen::Vector3d(0., 1., 2.);
};

struct TestServer
{
  TestServer();

  void publish();

  mc_control::ControllerServer server{1.0, 1.0, {"ipc:///tmp/mc_rtc_pub.ipc"}, {"ipc:///tmp/mc_rtc_rep.ipc"}};
  DummyProvider provider;
  mc_rtc::gui::StateBuilder builder;
  bool check_ = true;
  std::string string_ = "default";
  int int_ = 0;
  double d_ = 0.;
  double slide_ = 0.;
  Eigen::VectorXd v_{Eigen::VectorXd::Ones(6)};
  Eigen::Vector3d v3_{-1., -1., 1.};
  Eigen::Vector3d vInt_ = {0.2, 0.2, 0.};
  std::string combo_ = "b";
  std::string data_combo_;
  sva::PTransformd rotStatic_{sva::RotZ(-M_PI), Eigen::Vector3d(1., 1., 0.)};
  sva::PTransformd rotInteractive_{Eigen::Vector3d{0., 0., 1.}};
  sva::PTransformd static_{sva::RotZ(-M_PI), Eigen::Vector3d(1., 0., 0.)};
  sva::PTransformd interactive_{Eigen::Vector3d{0., 1., 0.}};
  Eigen::Vector3d xytheta_{0., 2., M_PI / 3};
  Eigen::VectorXd xythetaz_;
  std::vector<Eigen::Vector3d> polygon_;
  Eigen::Vector3d arrow_start_{0.5, 0.5, 0.};
  Eigen::Vector3d arrow_end_{0.5, 1., -0.5};
  sva::ForceVecd force_{{0., 0., 0.}, {-50., 50., 100.}};
};

TestServer::TestServer() : xythetaz_(4)
{
  xythetaz_ << 1., 2., M_PI / 5, 1;
  polygon_.push_back({1, 1, 0});
  polygon_.push_back({1, -1, 0});
  polygon_.push_back({-1, -1, 0});
  polygon_.push_back({-1, 1, 0});

  auto data = builder.data();
  data.add("DataComboInput", std::vector<std::string>{"Choice A", "Choice B", "Choice C", "Obiwan Kenobi"});
  data.add("robots", std::vector<std::string>{"Goldorak", "Astro"});
  auto bodies = data.add("bodies");
  bodies.add("Goldorak", std::vector<std::string>{"FulguroPoing", "FulguroPied"});
  bodies.add("Astro", std::vector<std::string>{"Head", "Arm", "Foot"});
  auto surfaces = data.add("surfaces");
  surfaces.add("Goldorak", std::vector<std::string>{"Goldo_S1", "Goldo_S2", "Goldo_S3"});
  surfaces.add("Astro", std::vector<std::string>{"Astro_S1", "Astro_S2", "Astro_S3"});
  auto joints = data.add("joints");
  joints.add("Goldorak", std::vector<std::string>{"Goldo_J1", "Goldo_J2", "Goldo_J3"});
  joints.add("Astro", std::vector<std::string>{"Astro_J1", "Astro_J2", "Astro_J3"});
  builder.addElement({}, mc_rtc::gui::Label("Test", []() { return 2; }));
  builder.addElement({"Test"}, mc_rtc::gui::Button("Test", []() { std::cout << "Test::Test clicked!\n"; }));
  builder.addElement({"Test", "data"},
                     mc_rtc::gui::Button("Test", []() { std::cout << "Test::Test::Test clicked!\n"; }));
  builder.addElement(
      {"dummy", "provider"}, mc_rtc::gui::Label("value", [this]() { return provider.value; }),
      mc_rtc::gui::ArrayLabel("point", [this]() { return provider.point; }),
      mc_rtc::gui::ArrayLabel("point with labels", {"x", "y", "z"}, [this]() { return provider.point; }));
  builder.addElement({"Button example"}, mc_rtc::gui::Button("Push me", []() { LOG_INFO("Button pushed") }));
  builder.addElement({"Stacked buttons"}, mc_rtc::gui::ElementsStacking::Horizontal,
                     mc_rtc::gui::Button("Foo", []() { LOG_INFO("Foo pushed") }),
                     mc_rtc::gui::Button("Bar", []() { LOG_INFO("Bar pushed") }));
  builder.addElement({"Checkbox example"},
                     mc_rtc::gui::Checkbox("Checkbox", [this]() { return check_; }, [this]() { check_ = !check_; }));
  builder.addElement({"StringInput example"}, mc_rtc::gui::StringInput("StringInput", [this]() { return string_; },
                                                                       [this](const std::string & data) {
                                                                         string_ = data;
                                                                         std::cout << "string_ changed to " << string_
                                                                                   << std::endl;
                                                                       }));
  builder.addElement({"IntegerInput example"}, mc_rtc::gui::IntegerInput("IntegerInput", [this]() { return int_; },
                                                                         [this](int data) {
                                                                           int_ = data;
                                                                           std::cout << "int_ changed to " << int_
                                                                                     << std::endl;
                                                                         }));
  builder.addElement({"NumberInput example"}, mc_rtc::gui::NumberInput("NumberInput", [this]() { return d_; },
                                                                       [this](double data) {
                                                                         d_ = data;
                                                                         std::cout << "d_ changed to " << d_
                                                                                   << std::endl;
                                                                       }));
  builder.addElement({"NumberSlider example"}, mc_rtc::gui::NumberSlider("NumberSlider", [this]() { return slide_; },
                                                                         [this](double s) {
                                                                           slide_ = s;
                                                                           std::cout << "slide_ changed to " << slide_
                                                                                     << std::endl;
                                                                         },
                                                                         -100.0, 100.0));
  builder.addElement({"ArrayInput example"}, mc_rtc::gui::ArrayInput("ArrayInput", [this]() { return v_; },
                                                                     [this](const Eigen::VectorXd & data) {
                                                                       v_ = data;
                                                                       std::cout << "v_ changed to " << v_.transpose()
                                                                                 << std::endl;
                                                                     }));
  builder.addElement({"ArrayInput with labels example"},
                     mc_rtc::gui::ArrayInput("ArrayInput with labels", {"x", "y", "z"}, [this]() { return v3_; },
                                             [this](const Eigen::Vector3d & data) {
                                               v3_ = data;
                                               std::cout << "v3_ changed to " << v3_.transpose() << std::endl;
                                             }));
  builder.addElement({"ComboInput"},
                     mc_rtc::gui::ComboInput("ComboInput", {"a", "b", "c", "d"}, [this]() { return combo_; },
                                             [this](const std::string & s) {
                                               combo_ = s;
                                               std::cout << "combo_ changed to " << combo_ << std::endl;
                                             }));
  builder.addElement({"DataComboInput"},
                     mc_rtc::gui::DataComboInput("DataComboInput", {"DataComboInput"}, [this]() { return data_combo_; },
                                                 [this](const std::string & s) {
                                                   data_combo_ = s;
                                                   std::cout << "data_combo_ changed to " << data_combo_ << std::endl;
                                                 }));
  builder.addElement({"Schema"}, mc_rtc::gui::Schema("Add metatask", "metatask", [](const mc_rtc::Configuration & c) {
                       std::cout << "Got schema request:\n" << c.dump(true) << std::endl;
                     }));
  builder.addElement(
      {"Contacts", "Add"},
      mc_rtc::gui::Form("Add contact",
                        [](const mc_rtc::Configuration & data) {
                          std::cout << "Got data" << std::endl << data.dump(true) << std::endl;
                        },
                        mc_rtc::gui::FormCheckbox{"Enabled", false, true},
                        mc_rtc::gui::FormIntegerInput{"INT", false, 42},
                        mc_rtc::gui::FormNumberInput{"NUMBER", false, 0.42},
                        mc_rtc::gui::FormStringInput{"STRING", false, "a certain string"},
                        mc_rtc::gui::FormArrayInput<Eigen::Vector3d>{"ARRAY_FIXED_SIZE", false, {1, 2, 3}},
                        mc_rtc::gui::FormArrayInput<std::vector<double>>{"ARRAY_UNBOUNDED", false},
                        mc_rtc::gui::FormComboInput{"CHOOSE WISELY", false, {"A", "B", "C", "D"}},
                        mc_rtc::gui::FormDataComboInput{"R0", false, {"robots"}},
                        mc_rtc::gui::FormDataComboInput{"R0 surface", false, {"surfaces", "$R0"}},
                        mc_rtc::gui::FormDataComboInput{"R1", false, {"robots"}},
                        mc_rtc::gui::FormDataComboInput{"R1 surface", false, {"surfaces", "$R1"}}));
  builder.addElement({"GUI Markers", "Transforms"},
                     mc_rtc::gui::Transform("ReadOnly Transform", [this]() { return static_; }),
                     mc_rtc::gui::Transform("Interactive Transform", [this]() { return interactive_; },
                                            [this](const sva::PTransformd & p) { interactive_ = p; }),
                     mc_rtc::gui::XYTheta("XYTheta", [this]() { return xytheta_; },
                                          [this](const Eigen::VectorXd & vec) { xytheta_ = vec.head<3>(); }),
                     mc_rtc::gui::XYTheta("XYThetaAltitude", [this]() { return xythetaz_; },
                                          [this](const Eigen::VectorXd & vec) { xythetaz_ = vec; }),
                     mc_rtc::gui::Rotation("ReadOnly Rotation", [this]() { return rotStatic_; }),
                     mc_rtc::gui::Rotation("Interactive Rotation", [this]() { return rotInteractive_; },
                                           [this](const Eigen::Quaterniond & q) { rotInteractive_.rotation() = q; }));

  builder.addElement(
      {"GUI Markers", "Point3D"},
      mc_rtc::gui::Point3D("ReadOnly", mc_rtc::gui::PointConfig({1., 0., 0.}, 0.08), [this]() { return v3_; }),
      mc_rtc::gui::Point3D("Interactive", mc_rtc::gui::PointConfig({0., 1., 0.}, 0.08), [this]() { return vInt_; },
                           [this](const Eigen::Vector3d & v) { vInt_ = v; }));

  mc_rtc::gui::ArrowConfig arrow_config({1., 0., 0.});
  arrow_config.start_point_scale = 0.02;
  arrow_config.end_point_scale = 0.02;
  builder.addElement(
      {"GUI Markers", "Arrows"},
      mc_rtc::gui::Arrow("ArrowRO", arrow_config,
                         []() {
                           return Eigen::Vector3d{2, 2, 0};
                         },
                         []() {
                           return Eigen::Vector3d{2.5, 2.5, 0.5};
                         }),
      mc_rtc::gui::Arrow("Arrow", arrow_config, [this]() { return arrow_start_; },
                         [this](const Eigen::Vector3d & start) { arrow_start_ = start; },
                         [this]() { return arrow_end_; }, [this](const Eigen::Vector3d & end) { arrow_end_ = end; }),
      mc_rtc::gui::Force("ForceRO", mc_rtc::gui::ForceConfig(mc_rtc::gui::Color(1., 0., 0.)),
                         []() {
                           return sva::ForceVecd(Eigen::Vector3d{0., 0., 0.}, Eigen::Vector3d{10., 0., 100.});
                         },
                         []() {
                           return sva::PTransformd{Eigen::Vector3d{2, 2, 0}};
                         }),
      mc_rtc::gui::Force("Force", mc_rtc::gui::ForceConfig(mc_rtc::gui::Color(0., 1., 0.)), [this]() { return force_; },
                         [this](const sva::ForceVecd & force) { force_ = force; },
                         []() {
                           return sva::PTransformd{Eigen::Vector3d{2, 2, 0}};
                         }));
}

void TestServer::publish()
{
  server.handle_requests(builder);
  server.publish(builder);
}

int main()
{
  TestServer server;
  while(1)
  {
    server.publish();
    std::this_thread::sleep_for(std::chrono::microseconds(50000));
  }
  return 0;
}
