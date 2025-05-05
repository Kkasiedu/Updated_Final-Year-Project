#ifndef MAPCANVAS_H
#define MAPCANVAS_H

#include <wx/wx.h>
#include <opencv4/opencv2/opencv.hpp> //Include OpenCV headers

class MapCanvas : public wxPanel {
public:
    MapCanvas(wxWindow* parent);
    void SetImage(const cv::Mat& img); // Update to use cv::Mat
    void SetZoomFactor(double factor) { zoomFactor = factor; Refresh(); }
    void ToggleGrid() { showGrid = !showGrid; Refresh(); }
    void SetZoom(double factor) { SetZoomFactor(factor); }
    void ResetPosition();
    void SaveCurrentView(const wxString& filePath);
    void SetMessage(const wxString& message);
    

private:
    cv::Mat image; // Update to use cv::Mat
    double zoomFactor = 1.0;
    bool showGrid = true;
    bool hasImage = false;

    wxButton* zoomInButton;
    wxButton* zoomOutButton;

    void OnPaint(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif // MAPCANVAS_H