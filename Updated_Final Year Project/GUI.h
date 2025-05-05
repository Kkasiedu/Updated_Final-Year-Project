#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>
#include "MapCanvas.h"
#include "SocketListener.h"

class GUI : public wxFrame {
public:
    GUI(const wxString& title);
    ~GUI();

private:
	//Menu Bar
    void OnOpenFolder(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnToggleOutputPanel(wxCommandEvent& event);
    void OnClearOutput(wxCommandEvent& event);
    void OnOpenImage(wxCommandEvent& event);
    void OnTreeSelectionChanged(wxTreeEvent& event);
    void OnToggleFileExplorer(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);

    //Receive Image
    void StartSocketListener();
    void StopSocketListener();
    void OnImageReceived(const std::string& filePath);
    void OnReceiveImage(wxCommandEvent& event); // Add this declaration

	//Video Stream
    void OnReceiveStream(wxCommandEvent& event);
    void ProcessFrame(const std::vector<char>& frameData);
   


    // AddTreeNodes declaration
    void AddTreeNodes(wxTreeItemId parentId, const wxString& folderPath);

    wxSplitterWindow* splitter;
    wxSplitterWindow* bottomSplitter;
    wxSplitterWindow* mapSplitter;
    wxPanel* fileExplorerPanel;
    wxPanel* outputPanel;
    wxTreeCtrl* fileTree;
    wxTextCtrl* outputText;
    MapCanvas* mapCanvas;
    wxImageList* imageList;
    wxStaticText* connectionStatus;

    cv::Mat originalImage;
    cv::Mat ndviImage;
    std::string rootFolderPath;
    bool isOutputPanelVisible;

    SocketListener* socketListener;
    std::string saveDirectory;
    int listeningPort;

    wxPanel* originalCanvas;
    wxPanel* ndviCanvas; // Added this line

    wxDECLARE_EVENT_TABLE();
};




#endif // GUI_H
