#include <experimental/filesystem>
#include <imgui.h>
#include <imgui_test/gl.h>
#include <map>
#include <opencv2/imgproc.hpp>

namespace imgui_test
{

struct Label;
struct Image;

struct Selection
{
  // upper left corner
  ImVec2 pt_;
  ImVec2 sz_;

  // std::weak_ptr<Image> source_image_;
  std::experimental::filesystem::path source_path_;
  std::string label_;
  cv::Mat sub_image_;
};

struct Image
{
  std::experimental::filesystem::path path_;

  bool dirty_ = true;
  bool load();
  std::string msg_;

  bool selected_ = false;
  cv::Mat image_;
  GLuint texture_id_ = 0;

  // map of all labels in this image
  std::map<std::string, std::vector<Selection> > selections_;
  // void export_selections(const size_t width, const size_t height);
};

struct Label
{
  std::string name_;
  std::string description_;

  // TODO(lucasw) color

  // TODO(lucasw) maybe an image example to show in gui?

  // TODO(lucasw) keyboard shortcut

  // TODO(lucasw) links to all Selections of this class?
};

class App
{
public:
  App(const ImVec2 size);

  void draw();

  std::string msg_;

  std::map<std::string, Label> labels_;
  std::string cur_label_;
  std::map<std::experimental::filesystem::path, std::shared_ptr<Image>> images_;
  std::experimental::filesystem::path cur_image_;
  bool droppedFile(const std::string name);
  bool loadDirectory(const std::string name);

  void drawControls();
  void drawFiles();
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

#if 0
  int width_ = 0;
  int height_ = 0;
#endif
  float mouse_wheel_ = 0.0;
  bool mouse_over_image_ = false;
  float zoom_ = 1.0;
  ImVec2 box_selection_size_ = ImVec2(128.0, 128.0);

  // float draw_col_[3] = {0.5, 0.5, 0.5};
  ImVec4 draw_col_ = ImVec4(0.5, 0.5, 0.5, 1.0);
  ImVec2 pos_;
  ImVec2 size_;
  ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;
};

}  // namespace imgui_test
