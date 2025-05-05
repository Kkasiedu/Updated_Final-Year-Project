#include "MapCanvas.h"
#include <wx/wx.h>

wxBEGIN_EVENT_TABLE(MapCanvas, wxPanel)
EVT_PAINT(MapCanvas::OnPaint)
EVT_MOTION(MapCanvas::OnMouseMove)
EVT_SIZE(MapCanvas::OnSize)
EVT_BUTTON(wxID_ANY, MapCanvas::OnZoomIn) // Bind the event handler
EVT_BUTTON(wxID_ANY, MapCanvas::OnZoomOut) // Bind the event handler
wxEND_EVENT_TABLE()

MapCanvas::MapCanvas(wxWindow* parent) : wxPanel(parent, wxID_ANY), hasImage(false) {
    SetBackgroundColour(wxColour(40, 40, 40));

    // Create zoom in and zoom out buttons
    zoomInButton = new wxButton(this, wxID_ANY, "+", wxPoint(10, 10), wxSize(30, 30));
    zoomOutButton = new wxButton(this, wxID_ANY, "-", wxPoint(50, 10), wxSize(30, 30));

    // Bind button events
    zoomInButton->Bind(wxEVT_BUTTON, &MapCanvas::OnZoomIn, this);
    zoomOutButton->Bind(wxEVT_BUTTON, &MapCanvas::OnZoomOut, this);
}

void MapCanvas::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    if (!hasImage) {
        dc.SetBackground(wxBrush(wxColour(40, 40, 40)));
        dc.Clear();
        dc.SetTextForeground(*wxWHITE);
        dc.SetFont(wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        wxString message = "Upload an Infrared Image to Start";
        wxCoord textWidth, textHeight;
        dc.GetTextExtent(message, &textWidth, &textHeight);
        dc.DrawText(message, (GetSize().GetWidth() - textWidth) / 2, (GetSize().GetHeight() - textHeight) / 2);
    }
    else if (!image.empty()) { // Updated check for cv::Mat
        wxImage scaledImage = wxImage(image.cols, image.rows, image.data, true).Scale(image.cols * zoomFactor, image.rows * zoomFactor, wxIMAGE_QUALITY_HIGH);
        wxBitmap bitmap(scaledImage);
        int xOffset = (GetSize().GetWidth() - bitmap.GetWidth()) / 2;
        int yOffset = (GetSize().GetHeight() - bitmap.GetHeight()) / 2;
        dc.DrawBitmap(bitmap, xOffset, yOffset);

        if (showGrid) {
            dc.SetPen(wxPen(*wxWHITE, 1, wxPENSTYLE_DOT));
            for (int x = 0; x < bitmap.GetWidth(); x += 50) {
                dc.DrawLine(x + xOffset, yOffset, x + xOffset, bitmap.GetHeight() + yOffset);
            }
            for (int y = 0; y < bitmap.GetHeight(); y += 50) {
                dc.DrawLine(xOffset, y + yOffset, bitmap.GetWidth() + xOffset, y + yOffset);
            }
        }
    }
}




void MapCanvas::SetImage(const cv::Mat& img) {
    image = img;
    hasImage = !image.empty();
    Refresh();
}


void MapCanvas::OnMouseMove(wxMouseEvent& event) {
    if (image.empty()) return; // Updated check for cv::Mat

    wxPoint pos = event.GetPosition();
    int xOffset = (GetSize().GetWidth() - image.cols * zoomFactor) / 2;
    int yOffset = (GetSize().GetHeight() - image.rows * zoomFactor) / 2;
    int imgX = (pos.x - xOffset) / zoomFactor;
    int imgY = (pos.y - yOffset) / zoomFactor;

    wxFrame* frame = wxDynamicCast(GetParent()->GetParent()->GetParent(), wxFrame);
    if (frame) {
        if (imgX >= 0 && imgX < image.cols && imgY >= 0 && imgY < image.rows) {
            wxString coordStr = wxString::Format("X: %d, Y: %d", imgX, imgY);
            frame->SetStatusText(coordStr, 0);
        }
        else {
            frame->SetStatusText("", 0);
        }
    }
}

void MapCanvas::SaveCurrentView(const wxString& filePath) {
    if (!hasImage || image.empty()) { // Updated check for cv::Mat
        wxMessageBox("No image to save.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxImage scaledImage = wxImage(image.cols, image.rows, image.data, true).Scale(image.cols * zoomFactor, image.rows * zoomFactor, wxIMAGE_QUALITY_HIGH);
    if (!scaledImage.SaveFile(filePath)) {
        wxMessageBox("Failed to save the image.", "Error", wxOK | wxICON_ERROR);
    }
}


void MapCanvas::ResetPosition() {
    // Reset the position logic
    // For example, you might want to reset the scroll position or any transformations
    // Here, we will just refresh the canvas
    zoomFactor = 1.0; // Reset zoom factor to default
    Refresh();
}



void MapCanvas::OnSize(wxSizeEvent& event) {
    Refresh(); // Refresh the canvas to redraw the text in the correct position
    event.Skip(); // Skip the event to allow default processing
}

void MapCanvas::OnZoomIn(wxCommandEvent& event) {
    zoomFactor *= 1.1; // Increase zoom factor by 10%
    Refresh();
}

void MapCanvas::OnZoomOut(wxCommandEvent& event) {
    zoomFactor /= 1.1; // Decrease zoom factor by 10%
    Refresh();
}


void MapCanvas::SetMessage(const wxString& message) {
    image.release(); // Clear any existing image
    hasImage = false;
    wxClientDC dc(this);
    dc.Clear();
    dc.DrawText(message, wxPoint(10, 10)); // Display the message at a specific position
    Refresh();
}



