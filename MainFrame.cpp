#include "MainFrame.h"
#include<wx/wx.h>
#include "resource.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_SOCKET(SOCKET_ID, MainFrame::OnSocketEvent)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	this->SetIcon(wxICON(aaaa));
	CreateControls();
	SetupSizers();
	BindEventHandlers();

	socket = nullptr;
}

void MainFrame::CreateControls()
{
	wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold());
	wxFont mainFont(wxFontInfo(wxSize(0, 14)));

	mainPanel = new wxPanel(this);
	mainPanel->SetFont(mainFont);

	headlineText = new wxStaticText(mainPanel, wxID_ANY, "BMS NetTest");
	headlineText->SetFont(headlineFont);

	ipInput = new wxTextCtrl(mainPanel, wxID_ANY, "");
	portInput = new wxTextCtrl(mainPanel, wxID_ANY, "");
	messageInput = new wxTextCtrl(mainPanel, wxID_ANY, "");

	ipInput->SetHint("127.0.0.1 (Target IP)");
	portInput->SetHint("2934 / 2935 / 9987-9989 (BMS Ports)");
	messageInput->SetHint("Test Message...");

	listenButton = new wxButton(mainPanel, wxID_ANY, "Listen");
	sendButton = new wxButton(mainPanel, wxID_ANY, "Send");

	logOutput = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
}

void MainFrame::SetupSizers()
{	
	//Main Sizer
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(headlineText, wxSizerFlags().CenterHorizontal().Border(wxTOP | wxBOTTOM, 10));

	//Input Sizers
	wxGridSizer* inputSizer = new wxGridSizer(2, 2, 5, 5);
	inputSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "IP Address:"), wxSizerFlags().Right());
	inputSizer->Add(ipInput, wxSizerFlags().Expand());
	inputSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Port:"), wxSizerFlags().Right());
	inputSizer->Add(portInput, wxSizerFlags().Expand());

	mainSizer->Add(inputSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	//Button Sizers
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(listenButton, wxSizerFlags().Expand().Proportion(1).Border(wxRIGHT, 5));
	buttonSizer->Add(sendButton, wxSizerFlags().Expand().Proportion(1).Border(wxLEFT, 5));

	mainSizer->Add(buttonSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	// Message input sizer
	wxBoxSizer* messageSizer = new wxBoxSizer(wxHORIZONTAL);
	messageSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Message:"), wxSizerFlags().Center().Border(wxRIGHT, 5));
	messageSizer->Add(messageInput, wxSizerFlags().Expand().Proportion(1));

	mainSizer->Add(messageSizer, wxSizerFlags().Expand().Border(wxALL, 5));

	// Log output
	mainSizer->Add(logOutput, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));

	mainPanel->SetSizerAndFit(mainSizer);
	mainSizer->SetSizeHints(this);
}

void MainFrame::BindEventHandlers()
{
	listenButton->Bind(wxEVT_BUTTON, &MainFrame::OnListenButtonClicked, this);
	sendButton->Bind(wxEVT_BUTTON, &MainFrame::OnSendButtonClicked, this);
	this->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent& evt) //On window close, Events will be stopped
		{
			evt.Skip();
		});
}

void MainFrame::OnListenButtonClicked(wxCommandEvent& evt)
{
	StartListening();
	if (!socket) {
		Log("Socket not initialized. Listen failed.");
		return;
	}
	socket->SetEventHandler(*this, SOCKET_ID);
	socket->SetNotify(wxSOCKET_INPUT_FLAG);
	socket->Notify(true);
}

void MainFrame::OnSendButtonClicked(wxCommandEvent& evt)
{
	SendPacket();
}

void MainFrame::OnSocketEvent(wxSocketEvent& evt) // AI generated code from here
{
	wxSocketBase* sock = evt.GetSocket();

	switch (evt.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
	{
		// Downcast the base socket to a datagram socket
		wxDatagramSocket* datagram_sock = static_cast<wxDatagramSocket*>(sock);

		if (datagram_sock)
		{
			wxIPV4address peer_addr;
			char buffer[1024]; // Buffer for incoming data

			// Now, call RecvFrom on the datagram socket
			datagram_sock->RecvFrom(peer_addr, buffer, sizeof(buffer) - 1);
			long bytes_read = datagram_sock->LastCount();

			if (bytes_read > 0)
			{
				buffer[bytes_read] = '\0';
				Log(wxString::Format("Received %d bytes from %s:%d: %s",
					bytes_read,
					peer_addr.Hostname(),
					peer_addr.Service(),
					wxString(buffer)));
			}
		}
		break;
	}
	}
}

void MainFrame::StartListening() 
{
	long port_number;
	if (!portInput->GetValue().ToLong(&port_number))
	{
		Log("Error: Invalid port number.");
		return;
	}

	if (socket) {
		socket->Destroy();
		socket = nullptr;
	}

	wxIPV4address addr;
	addr.Service(port_number);

	socket = new wxDatagramSocket(addr);

	if (!socket->Ok())
	{
		Log(wxString::Format("Error: Failed to create and bind socket. (Port %d)", port_number));
		delete socket;
		  socket = nullptr;
		  return;
	}

	socket->SetEventHandler(*this, SOCKET_ID);
	socket->SetNotify(wxSOCKET_INPUT_FLAG);
	socket->Notify(true);

	Log(wxString::Format("Listening on port %d...", port_number));
}

void MainFrame::SendPacket()
{
	wxString ip_address = ipInput->GetValue();
	long port_number;
	if (!portInput->GetValue().ToLong(&port_number))
	{
		Log("Error: Invalid port number.");
		return;
	}

	wxString message = messageInput->GetValue();

	if (message.IsEmpty()) {
		Log("Error: Message cannot be empty.");
		return;
	}

	wxIPV4address local_addr;
	local_addr.AnyAddress(); // Bind to any local address
	local_addr.Service(0);   // Let OS pick a free port

	wxDatagramSocket send_socket(local_addr);

	wxIPV4address dest_addr;
	dest_addr.Hostname(ip_address);
	dest_addr.Service(port_number);

	// The `SendTo` method returns a reference to the socket.
	// We then chain the `LastCount` call to get the number of bytes sent.
	// A return value of -1 or any negative number indicates an error.
	if (send_socket.SendTo(dest_addr, message.mb_str(), message.length()).LastCount() < 0)
	{
		Log("Error: Failed to send packet.");
	}
	else
	{
		Log(wxString::Format("Sent message to %s:%d", ip_address, port_number));
	}
}

void MainFrame::Log(const wxString& message)
{
	wxDateTime now = wxDateTime::Now();
	wxString formatted_message = now.Format("%H:%M:%S") + " - " + message + "\n";
	logOutput->AppendText(formatted_message);
}
