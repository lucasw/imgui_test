#include <experimental/filesystem>
#include <imgui_test/app.hpp>
#include <imgui_test/utility.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <set>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

// the experimental goes away in g++ 8
namespace fs = std::experimental::filesystem;

namespace imgui_test
{

App::App(const ImVec2 size) : size_(size)
{
  pos_.x = 0;
  pos_.y = 0;

  window_flags_ = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_HorizontalScrollbar;  // | ImGuiWindowFlags_NoDecoration;
}

void App::drawControls()
{
  // ImGui::Columns(2);
  #if 0
  float tmp = width_;
  const bool width_changed = ImGui::SliderFloat("width", &tmp, 1.0, 512.0, "%1.0f", 2.0);
  width_ = tmp;
  tmp = height_;
  const bool height_changed = ImGui::SliderFloat("height", &tmp, 1.0, 512.0, "%1.0f", 2.0);
  height_ = tmp;
  #endif
  const bool zoom_changed = ImGui::SliderFloat("zoom", &zoom_, 0.1, 64.0, "%1.1f", 2.0);
  (void)zoom_changed;

  #if 0
  if (width_changed || height_changed) {
    cv::resize(image_, image_, cv::Size(width_, height_), 0.0, 0.0, cv::INTER_NEAREST);
    // dirty = true;
    glTexFromMat(image_, texture_id_);
  }
  #endif

  ImGui::ColorEdit4("draw color", &draw_col_.x);

  // ImGui::NextColumn();
  // ImGui::Columns(1);
  ImGui::Text("%s", cur_image_.filename().c_str());
  ImGui::Text("%s", msg_.c_str());
  if (images_.count(cur_image_) > 0) {
    ImGui::Text("%s", images_[cur_image_]->msg_.c_str());
  }
  ImGui::Separator();
  {
    // ImGui::Columns(2, NULL, false);
    for (auto& pair : images_) {
      const auto filename = pair.first.filename();
      if (ImGui::Selectable(filename.c_str(), pair.second->selected_)) {
        // std::cout << filename << " selected\n";
        cur_image_ = pair.first;
        images_[cur_image_]->load();
      }
    }
    // ImGui::Columns(1);
  }
}

void App::drawImage()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  if (images_.count(cur_image_) < 1) {
    return;
  }

  cv::Mat image = images_[cur_image_]->image_;
  auto& texture_id = images_[cur_image_]->texture_id_;

  if (image.empty()) {
    return;
  }

  ImVec2 win_sz = ImGui::GetWindowSize();
  bool dirty = false;

  const float region_width = ImGui::GetWindowContentRegionWidth();
  const float rel_zoom_x = static_cast<float>(region_width) / image.cols;
  const float region_height = win_sz.y - 16;
  const float rel_zoom_y = static_cast<float>(region_height) / image.rows;
  const float rel_zoom = (rel_zoom_x > rel_zoom_y) ? rel_zoom_y : rel_zoom_x;
  ImVec2 image_size;
  image_size.x = image.cols * zoom_ * rel_zoom;
  image_size.y = image.rows * zoom_ * rel_zoom;

  // const ImVec2 uv0(-10.0, -10.0);  // = win_sz * 0.5 - image_size * 0.5;
  // const ImVec2 uv1(10.0, 10.0);  //  = win_sz * 0.5 + image_size * 0.5;
  // ImGui::Text("%f %f, %f %f", win_sz.x, win_sz.y, pos.x, pos.y);
  if (image_size.x < region_width) {
    ImGui::SetCursorPosX((region_width - image_size.x) * 0.5);
  }
  // float region_height = ImGui::GetWindowContentRegionHeight();
  if (image_size.y < region_height) {
    ImGui::SetCursorPosY((region_height - image_size.y) * 0.5);
  }
  ImVec2 screen_pos1 = ImGui::GetCursorScreenPos();
  ImGui::Image((void*)(intptr_t)texture_id, image_size);  // , uv0, uv1);

  ImGuiWindow* window = ImGui::GetCurrentWindow();
  ImGuiID active_id = ImGui::GetActiveID();
  const bool any_scrollbar_active = active_id &&
      (active_id == ImGui::GetScrollbarID(window, ImGuiAxis_X) ||
       active_id == ImGui::GetScrollbarID(window, ImGuiAxis_Y));

  if (!any_scrollbar_active) {
    ImVec2 image_pos = (io.MousePos - screen_pos1) / (zoom_ * rel_zoom);
    int x = image_pos.x;
    int y = image_pos.y;
    cv::Vec3b col(draw_col_.z * 255, draw_col_.y * 255, draw_col_.x * 255);
    bool clicked = ImGui::IsMouseDown(0);
    if ((x >= 0) && (y >= 0) && (x < image.cols) && (y < image.rows)) {
      // mouse_over_image = true;
      if (clicked) {
        // image.at<cv::Vec3b>(y, x) = col;
        // dirty = true;
      }
      hovered_col_ = image.at<cv::Vec3b>(y, x);
      hovered_x_ = x;
      hovered_y_ = y;
    }
  }

  if (dirty) {
    // image_ = cv::Mat(cv::Size(width_, height_), CV_8UC3);
    glTexFromMat(image, texture_id);
  }

  // draw a box where the mouse currently is
  {
    const size_t hwd = 50;
    const size_t hht = hwd;
    ImVec2 p = io.MousePos;
    const std::vector<ImVec2> corners = {
        ImVec2(p.x - hwd, p.y - hht),
        ImVec2(p.x - hwd, p.y + hht),
        ImVec2(p.x + hwd, p.y + hht),
        ImVec2(p.x + hwd, p.y - hht)
    };
    for (size_t i = 0; i < corners.size(); ++i) {
      ImGui::GetWindowDrawList()->AddLine(
          corners[i],
          corners[(i + 1) % corners.size()],
          IM_COL32(0, 0, 0, 208), 4.0f);

      ImGui::GetWindowDrawList()->AddLine(
          corners[i],
          corners[(i + 1) % corners.size()],
          IM_COL32(255, 0, 0, 208), 2.0f);
    }
  }

  // TODO(lucasw) if clicked, translate screen coords to image coords,
  // store those coords in a map of vectors within the Image.
}

void App::displayImageInfo()
{
  // Draw small box with the hovered color in it
  #if 0
  if (image_filename_ != "") {
    fs::path path = image_filename_;
    ImGui::Text("%s", path.filename().c_str());
  }
  #endif
  ImGui::Text("%04d %04d, r %02x g %02x b %02x",
      hovered_x_, hovered_y_,
      hovered_col_[0], hovered_col_[1], hovered_col_[2]);
      // clicked);
}

// TODO(lucasw) delete this or put it in a utility file somewhere
void App::drawPrimitives()
{
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  // draw primitives
  ImVec2 pt = ImGui::GetCursorScreenPos();
  const ImU32 col32 = ImColor(draw_col_);
  // const float spacing = 30.0;
  const float sz = 4.0;
  // can't draw too many
  // imgui_test: /home/lwalter/own/imgui_test/imgui/imgui.cpp:3937: void AddDrawListToDrawData(ImVector<ImDrawList*>*, ImDrawList*): Assertion `draw_list->_VtxCurrentIdx < (1 << 16) && "Too many vertices in ImDrawList using 16-bit indices. Read comment above"' failed.
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      const float cx = pt.x + j * sz * 1.5;
      const float cy = pt.y + i * sz * 1.5;
      // draw_list->AddCircle(ImVec2(cx, cy), 7.0, col32, 4, 4.0);
      draw_list->AddRectFilled(ImVec2(cx, cy), ImVec2(cx + sz, cy + sz), col32);
    }
  }
}

void App::draw()
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  bool is_open = true;

  ImVec2 pos, sz;

  {
    pos = pos_;
    sz = ImVec2(size_.x * 0.25, size_.y * 0.9);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(sz);
    ImGui::Begin("controls", &is_open, window_flags_);
    if (is_open) {
      drawControls();
    }
    ImGui::End();
  }

  {
    pos = ImVec2(pos.x, pos.y + sz.y);
    sz = ImVec2(sz.x, size_.y - sz.y);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(sz);
    ImGui::Begin("image info", &is_open, window_flags_);
    displayImageInfo();
    ImGui::End();
  }

  {
    // bool mouse_over_image = false;
    pos = ImVec2(pos.x + sz.x, 0);
    sz = ImVec2(size_.x - sz.x, size_.y);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(sz);
    ImGui::Begin("image display", &is_open, window_flags_);

    if (is_open) {
      drawImage();
    }
    ImGui::End();
  }
#if 0
  // if (mouse_over_image)
  {
    auto wheel_diff = io.MouseWheel;
    zoom_ += wheel_diff * 0.5;
    // ImGui::Text("zoom %f %f %f", zoom_, wheel_diff, io.MouseWheel);
  }
  mouse_wheel_ = io.MouseWheel;

  mouse_over_image_ = mouse_over_image;
#endif
}

bool Image::load()
{
  if (!dirty_) {
    return true;
  }
  std::string name = path_.string();

  if (name == "") return false;
  cv::Mat image = cv::imread(name, cv::IMREAD_COLOR);
  if (image.empty()) {
    msg_ = "Could not load image '" + name + "'";
    return false;
  }
  GLuint texture_id;
  const bool rv = glTexFromMat(image, texture_id);

  if (rv) {
    image_ = image;
    // width_ = image_.cols;
    // height_ = image_.rows;
    texture_id_ = texture_id;
  }

  dirty_ = false;
  return rv;
}

bool App::droppedFile(const std::string name)
{
  return loadDirectory(name);
}

bool App::loadDirectory(const std::string name)
{
  if (name == "") {
    return false;
  }
  msg_ = "loading directory of '" + name + "'";
  std::cout << msg_ << "\n";

  // get a list of all image from current directory
  try {
    const std::set<std::string> image_extensions = {".png", ".jpg", ".jpeg"};
    // TODO(lucasw) ought to keep around the old images at least for a while,
    // but for now dispense with them
    std::map<std::experimental::filesystem::path, std::shared_ptr<Image>> images;
    fs::path path = name;
    const auto dir = path.parent_path();
    std::cout << dir << ":\n";
    for (const auto& entry : fs::directory_iterator(dir)) {
      if (!fs::is_regular_file(entry.path())) {
        continue;
      }
      if (image_extensions.count(entry.path().extension()) < 1) {
        continue;
      }
      auto image = std::make_shared<Image>();
      image->path_ = entry.path();
      images[image->path_] = image;

      // std::cout << entry.path().filename() << " " << entry.path().extension() << "\n";
    }

    images_ = images;
  } catch (std::runtime_error& ex) {
    std::cerr << ex.what() << "\n";
    return false;
  }

  return true;
}

}  // namespace imgui_test
