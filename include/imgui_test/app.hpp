#include <imgui.h>
#include <imgui_test/gl.h>
#include <opencv2/imgproc.hpp>

namespace imgui_test
{

class App
{
public:
  App(const ImVec2 size);

  void draw();

  std::string msg_;
  std::string image_filename_;
  bool droppedFile(const std::string name);

  void drawControls();
  void drawImage();
  // draw the image that lies within the currently selected box,
  // or what would become the next image box if the user creates one at the current
  // hovered position.
  // void drawImageBox();
  void displayImageInfo();
  void drawPrimitives();

  cv::Vec3b hovered_col_;
  int hovered_x_ = 0;
  int hovered_y_ = 0;

  cv::Mat image_;
  float mouse_wheel_ = 0.0;
  bool mouse_over_image_ = false;
  int width_ = 16;
  int height_ = 16;
  float zoom_ = 8.0;

  // float draw_col_[3] = {0.5, 0.5, 0.5};
  ImVec4 draw_col_ = ImVec4(0.5, 0.5, 0.5, 1.0);
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;

  GLuint texture_id_;
};

}  // namespace imgui_test
