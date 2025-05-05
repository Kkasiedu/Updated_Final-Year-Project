#include "GUI.h"
#include <wx/filedlg.h>
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>
#include <wx/artprov.h>
#include <wx/datetime.h>
#include <wx/msgdlg.h>

enum {
    ID_OPEN_FOLDER = wxID_HIGHEST + 1,
    ID_SAVE_FILE,
    ID_EXIT,
    ID_TOGGLE_OUTPUT_PANEL,
    ID_CLEAR_OUTPUT,
    ID_OPEN_IMAGE,
    ID_TOGGLE_FILE_EXPLORER,
    ID_SETTINGS,
    ID_RECEIVE_IMAGE, // New menu item ID
	ID_RECEIVE_STREAM  
};

wxBEGIN_EVENT_TABLE(GUI, wxFrame)
EVT_MENU(ID_OPEN_FOLDER, GUI::OnOpenFolder)
EVT_MENU(ID_SAVE_FILE, GUI::OnSaveFile)
EVT_MENU(ID_EXIT, GUI::OnExit)
EVT_MENU(ID_TOGGLE_OUTPUT_PANEL, GUI::OnToggleOutputPanel)
EVT_MENU(ID_CLEAR_OUTPUT, GUI::OnClearOutput)
EVT_MENU(ID_OPEN_IMAGE, GUI::OnOpenImage)
EVT_MENU(ID_TOGGLE_FILE_EXPLORER, GUI::OnToggleFileExplorer)
EVT_MENU(ID_SETTINGS, GUI::OnSettings)
EVT_TREE_SEL_CHANGED(wxID_ANY, GUI::OnTreeSelectionChanged)
EVT_MENU(ID_RECEIVE_IMAGE, GUI::OnReceiveImage)
EVT_MENU(ID_RECEIVE_STREAM, GUI::OnReceiveStream)
wxEND_EVENT_TABLE()

GUI::GUI(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    originalImage(), ndviImage(), mapSplitter(nullptr), originalCanvas(nullptr), ndviCanvas(nullptr), rootFolderPath(),
    isOutputPanelVisible(true), saveDirectory("./received_images"), listeningPort(5001), socketListener(nullptr) {
    SetBackgroundColour(wxColour("#D3D3D3"));

    // Menu Bar
    wxMenuBar* menuBar = new wxMenuBar;

    // File Menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_OPEN_FOLDER, "&Open Folder\tCtrl+O");
    fileMenu->Append(ID_SAVE_FILE, "&Save\tCtrl+S");
    fileMenu->Append(ID_OPEN_IMAGE, "&Open Image\tCtrl+I");
    fileMenu->Append(ID_RECEIVE_IMAGE, "&Receive Image\tCtrl+R");
    menuBar->Append(fileMenu, "&File");

    // Run Menu
    wxMenu* runMenu = new wxMenu;
    runMenu->Append(ID_CLEAR_OUTPUT, "&Clear Output");
    menuBar->Append(runMenu, "&Run");

    // Video Menu
    wxMenu* videoMenu = new wxMenu;
    videoMenu->Append(ID_RECEIVE_STREAM, "&Receive Stream\tCtrl+V");
    menuBar->Append(videoMenu, "&Video");

    // Exit Menu
    wxMenu* exitMenu = new wxMenu;
    exitMenu->Append(ID_EXIT, "E&xit\tCtrl+Q");
    menuBar->Append(exitMenu, "&Exit");

    // View Menu
    wxMenu* viewMenu = new wxMenu;
    viewMenu->AppendCheckItem(ID_TOGGLE_OUTPUT_PANEL, "Toggle Output Panel");
    viewMenu->AppendCheckItem(ID_TOGGLE_FILE_EXPLORER, "Toggle File Explorer");
    menuBar->Append(viewMenu, "&View");

    // Settings Menu
    wxMenu* settingsMenu = new wxMenu;
    settingsMenu->Append(ID_SETTINGS, "&Settings");
    menuBar->Append(settingsMenu, "&Settings");

    SetMenuBar(menuBar);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Splitter Window
    splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
    bottomSplitter = new wxSplitterWindow(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

    // File Explorer Panel
    fileExplorerPanel = new wxPanel(splitter, wxID_ANY);
    fileExplorerPanel->SetBackgroundColour(wxColour("#D3D3D3"));
    wxBoxSizer* fileExplorerSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* fileExplorerTitle = new wxStaticText(fileExplorerPanel, wxID_ANY, "File Explorer");
    fileExplorerSizer->Add(fileExplorerTitle, 0, wxEXPAND | wxALL, 5);

    imageList = new wxImageList(16, 16, true);
    wxIcon folderIcon = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
    wxIcon fileIcon = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
    imageList->Add(folderIcon);
    imageList->Add(fileIcon);

    fileTree = new wxTreeCtrl(fileExplorerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
    fileTree->AssignImageList(imageList);
    fileExplorerSizer->Add(fileTree, 1, wxEXPAND | wxALL, 5);
    fileExplorerPanel->SetSizer(fileExplorerSizer);

    // Output Panel
    outputPanel = new wxPanel(bottomSplitter, wxID_ANY);
    outputPanel->SetBackgroundColour(wxColour("#D3D3D3"));
    wxBoxSizer* outputSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* outputTitle = new wxStaticText(outputPanel, wxID_ANY, "Output");
    outputSizer->Add(outputTitle, 0, wxEXPAND | wxALL, 5);

    outputText = new wxTextCtrl(outputPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    outputSizer->Add(outputText, 1, wxEXPAND | wxALL, 5);
    outputPanel->SetSizer(outputSizer);

    // Connection Status (initially empty)
    connectionStatus = new wxStaticText(outputPanel, wxID_ANY, "");
    outputSizer->Add(connectionStatus, 0, wxEXPAND | wxALL, 5);

    mapCanvas = new MapCanvas(bottomSplitter);
    bottomSplitter->SplitHorizontally(mapCanvas, outputPanel, 400);
    splitter->SplitVertically(fileExplorerPanel, bottomSplitter, 250);

    mainSizer->Add(splitter, 1, wxEXPAND);

    SetSizerAndFit(mainSizer);

    // Hide the file explorer by default
    splitter->Unsplit(fileExplorerPanel);

    CreateStatusBar();
    SetStatusText("Ready");

    Show(true);
}



GUI::~GUI() {
    StopSocketListener();
}

void GUI::OnOpenFolder(wxCommandEvent& event) {
    wxDirDialog dirDialog(this, "Select a folder containing images", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    rootFolderPath = dirDialog.GetPath();
    wxDir dir(rootFolderPath);
    if (!dir.IsOpened()) {
        wxMessageBox(wxString::Format("Failed to open folder: %s", rootFolderPath), "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxFileName folderName(rootFolderPath);
    wxString folderNameOnly = folderName.GetFullName();

    fileTree->DeleteAllItems();
    wxTreeItemId rootId = fileTree->AddRoot(folderNameOnly, 0);
    AddTreeNodes(rootId, rootFolderPath);
    fileTree->ExpandAll();
}

void GUI::AddTreeNodes(wxTreeItemId parentId, const wxString& folderPath) {
    wxDir dir(folderPath);
    if (!dir.IsOpened()) {
        return;
    }

    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont) {
        wxString subfolderPath = folderPath + wxFILE_SEP_PATH + filename;
        wxTreeItemId subfolderId = fileTree->AppendItem(parentId, filename, 0);
        AddTreeNodes(subfolderId, subfolderPath);
        cont = dir.GetNext(&filename);
    }

    cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont) {
        fileTree->AppendItem(parentId, filename, 1);
        cont = dir.GetNext(&filename);
    }
}

void GUI::OnOpenImage(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Open Image", "", "", "Image files (*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    wxString filePath = openFileDialog.GetPath();
    wxLogMessage("Opening image: %s", filePath);
    cv::Mat img = cv::imread(filePath.ToStdString(), cv::IMREAD_COLOR);
    if (img.empty()) {
        wxMessageBox("Failed to load the image.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    wxLogMessage("Image loaded successfully");
    originalImage = img;
    mapCanvas->SetImage(img);
    SetStatusText("Image Loaded Successfully");
}

void GUI::OnTreeSelectionChanged(wxTreeEvent& event) {
    wxTreeItemId selectedId = event.GetItem();
    if (!selectedId.IsOk()) {
        return;
    }

    wxString relativePath = fileTree->GetItemText(selectedId);
    wxTreeItemId parentId = fileTree->GetItemParent(selectedId);
    while (parentId.IsOk() && parentId != fileTree->GetRootItem()) {
        relativePath = fileTree->GetItemText(parentId) + wxFILE_SEP_PATH + relativePath;
        parentId = fileTree->GetItemParent(parentId);
    }

    wxString fullPath = rootFolderPath + wxFILE_SEP_PATH + relativePath;

    wxLogMessage("Selected file path: %s", fullPath);

    if (wxFileExists(fullPath)) {
        cv::Mat img = cv::imread(fullPath.ToStdString(), cv::IMREAD_COLOR);
        if (img.empty()) {
            wxMessageBox("Failed to load the image.", "Error", wxOK | wxICON_ERROR);
            return;
        }
        wxLogMessage("Image loaded successfully");
        mapCanvas->SetImage(img);
        SetStatusText("Image Loaded Successfully");
    }
    else {
        wxLogMessage("File does not exist: %s", fullPath);
    }
}

void GUI::OnSaveFile(wxCommandEvent& event) {
    // Implement save functionality
}

void GUI::OnExit(wxCommandEvent& event) {
    Close(true);
}

void GUI::OnToggleOutputPanel(wxCommandEvent& event) {
    isOutputPanelVisible = !isOutputPanelVisible;
    if (isOutputPanelVisible) {
        bottomSplitter->SplitHorizontally(mapCanvas, outputPanel, 400);
    }
    else {
        bottomSplitter->Unsplit(outputPanel);
    }
    bottomSplitter->UpdateSize();
}

void GUI::OnClearOutput(wxCommandEvent& event) {
    wxDateTime startTime = wxDateTime::Now();
    outputText->Clear();
    wxDateTime endTime = wxDateTime::Now();
    wxTimeSpan duration = endTime - startTime;
    wxString message = wxString::Format("Output Cleared! Time: %s Duration: %s", endTime.FormatISOTime(), duration.Format("%H:%M:%S"));
    outputText->AppendText(message);
}

void GUI::OnToggleFileExplorer(wxCommandEvent& event) {
    if (splitter->IsSplit()) {
        splitter->Unsplit(fileExplorerPanel);
    }
    else {
        splitter->SplitVertically(fileExplorerPanel, bottomSplitter, 250);
    }
}

void GUI::OnSettings(wxCommandEvent& event) {
    wxTextEntryDialog portDialog(this, "Enter listening port:", "Settings", wxString::Format("%d", listeningPort));
    if (portDialog.ShowModal() == wxID_OK) {
        long port;
        if (portDialog.GetValue().ToLong(&port) && port > 0 && port < 65536) {
            listeningPort = static_cast<int>(port);
        }
        else {
            wxMessageBox("Invalid port number.", "Error", wxOK | wxICON_ERROR);
        }
    }

    wxDirDialog dirDialog(this, "Select save directory:", saveDirectory);
    if (dirDialog.ShowModal() == wxID_OK) {
        saveDirectory = dirDialog.GetPath().ToStdString();
    }

    StopSocketListener();
    StartSocketListener();
}

void GUI::StartSocketListener() {
    if (!socketListener) {
        socketListener = new SocketListener("0.0.0.0", listeningPort, saveDirectory);
        socketListener->setOnImageReceivedCallback([this](const std::string& filePath) {
            wxTheApp->CallAfter([this, filePath]() {
                OnImageReceived(filePath);
                });
            });
        socketListener->start();
        connectionStatus->SetLabel("Listening for images...");
        wxLogMessage("Socket listener started on port %d", listeningPort);
    }
    else {
        wxLogMessage("Socket listener is already running.");
    }
}

void GUI::StopSocketListener() {
    if (socketListener) {
        socketListener->stop();
        delete socketListener;
        socketListener = nullptr;
    }
}

void GUI::OnImageReceived(const std::string& filePath) {
    wxString message = wxString::Format("Image received: %s", filePath);
    outputText->AppendText(message);
    connectionStatus->SetLabel(""); // Clear the waiting message
    wxLogMessage(message);
}


void GUI::OnReceiveImage(wxCommandEvent& event) {
    StartSocketListener();

    // Update the MapCanvas to display "Waiting to Receive Image..."
    if (mapCanvas) {
        mapCanvas->SetMessage("Waiting to Receive Image...");
    }

    // Update the connection status in the output panel
    connectionStatus->SetLabel("Waiting for image...");
}


void GUI::OnReceiveStream(wxCommandEvent& event) {
    wxMessageBox("Video streaming functionality is not yet implemented.", "Info", wxOK | wxICON_INFORMATION);
}
