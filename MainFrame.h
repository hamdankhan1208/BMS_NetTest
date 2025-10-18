#pragma once
#include<wx/wx.h>
#include<wx/socket.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	// Helper methods for initialization
	void CreateControls();
	void SetupSizers();
	void BindEventHandlers();

	//Event Handlers
	void OnListenButtonClicked(wxCommandEvent& evt);
	void OnSendButtonClicked(wxCommandEvent& evt);
	void OnSocketEvent(wxSocketEvent& evt);

	//Helper
	void StartListening();
	void SendPacket();
	void Log(const wxString& message);

	// GUI controls
	wxPanel* mainPanel;
	wxStaticText* headlineText;
	wxTextCtrl* ipInput;
	wxTextCtrl* portInput;
	wxTextCtrl* messageInput;
	wxButton* listenButton;
	wxButton* sendButton;
	wxTextCtrl* logOutput;

	// Network objects
	wxDatagramSocket* socket;

	DECLARE_EVENT_TABLE()
};
//Unique ID for socket
enum
{
	SOCKET_ID = wxID_HIGHEST + 1
};

