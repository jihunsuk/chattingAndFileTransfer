// FileLayer.cpp: implementation of the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "FileLayer.h"
#include "IPCAppDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLayer::CFileLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
	m_ackReady = FALSE;
	BooleanErr = TRUE;
	RecEnd = FALSE;
}

CFileLayer::~CFileLayer()
{
}

void CFileLayer::ResetHeader()
{
	m_sHeader.fapp_totlen = 0;
	m_sHeader.fapp_type = 0;
	m_sHeader.fapp_msg_type = 0;
	m_sHeader.ed = 0;
	m_sHeader.fapp_seq_num = 0;
	memset(m_sHeader.fapp_data, 0, FILE_DATA_SIZE);
}

BOOL CFileLayer::Send(unsigned char *ppayload, int nlength, int port)
{

	memset(filebuf, 0, FILE_DATA_SIZE);
	errReady = 0;
	CString filepath;
	filepath.Format("%s", ppayload);

	CString filename = filepath.Right(filepath.GetLength() - filepath.ReverseFind('\\') - 1);
	TRY
	{
		CFile m_fileDes(filepath, CFile::modeRead);
		FILE_APP_INFO fileInfo;

		memset(fileInfo.fileName, '\0', 255);//setting FileName
		memcpy(fileInfo.fileName, filename.GetBuffer(0), filename.GetLength());
		fileInfo.totalLength = m_fileDes.GetLength();
		// init progressbar
		((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetRange32(0, (int) m_fileDes.GetLength());
		((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos(0);


		m_sHeader.fapp_totlen = FILE_INFO_SIZE;
		m_sHeader.fapp_type = DATA_HEAD;
		m_sHeader.fapp_msg_type = MSG_DATA;
		m_sHeader.ed = 0;
		m_sHeader.fapp_seq_num = 0;
		memcpy(m_sHeader.fapp_data, (unsigned char*) &fileInfo, FILE_INFO_SIZE);
		//end to setting, send readyPacket to Receiver
		mp_UnderLayer->Send((unsigned char*) &m_sHeader, FILE_INFO_SIZE + FILE_HEADER_SIZE, FILE_TYPE);

		while (!m_ackReady);//wait to receive returnPacket

		unsigned int sendLength = 0;//init sendLength

		// if filesize over FILE_DATA_SIZE, go plagmentation
		if (m_fileDes.GetLength() > FILE_DATA_SIZE) {
			totalLength = 0;
			m_sHeader.fapp_type = DATA_MAIN;
			m_sHeader.fapp_msg_type = MSG_DATA;

			//while(do) send all of file data
			while (m_fileDes.GetLength() > FILE_DATA_SIZE) {
				m_sHeader.ed = 0;

				if(!((CIPCAppDlg*) mp_aUpperLayer[0])->FileSended){//file cancle
					m_sHeader.fapp_type = MSG_CANCLE;//Send MSG_COR(correct) to Receiver
					mp_UnderLayer->Send((unsigned char*) &m_sHeader, FILE_HEADER_SIZE, FILE_TYPE);
					m_fileDes.Close();//end file send
					m_ackReady = FALSE;
					BooleanErr = TRUE;
					RecEnd = FALSE;
					AfxMessageBox("파일전송취소!");
					return TRUE;
				}

				if (errReady){//if take ErrPacket, process to resend file's data
					totalLength = m_sHeader.fapp_seq_num * FILE_DATA_SIZE; // recalculate totalLength(it's a SuccessPacket's num * FILE_DATA_SIZE)
					m_fileDes.Seek(totalLength, 0);//move file's pointer to seq_num
					errReady = 0;//decheck to error's value = 0
					m_sHeader.fapp_type = MSG_COR;//Send MSG_COR(correct) to Receiver
					mp_UnderLayer->Send((unsigned char*) &m_sHeader, FILE_HEADER_SIZE, FILE_TYPE);

					m_sHeader.fapp_type = DATA_MAIN;

					CString message;//display console to "Error"
					message.Format("Error : %d segment", m_sHeader.fapp_seq_num);
					int index = ((CIPCAppDlg*) mp_aUpperLayer[0])->m_ListChat.AddString(message);
					((CIPCAppDlg*) mp_aUpperLayer[0])->m_ListChat.SetCurSel(index);
				}

				//Read File data and copy to filebuf
				sendLength = m_fileDes.Read(filebuf, FILE_DATA_SIZE); // (fileInfo.totalLength - totalLength) > FILE_DATA_SIZE ? FILE_DATA_SIZE: (fileInfo.totalLength - totalLength)

				m_sHeader.fapp_totlen = sendLength;//legnth is sendLength
				memcpy(m_sHeader.fapp_data, filebuf, sendLength);

				mp_UnderLayer->Send((unsigned char*) &m_sHeader, sendLength + FILE_HEADER_SIZE, FILE_TYPE);
				m_sHeader.fapp_seq_num++;	//seq_num add 1
				totalLength += sendLength; // add sendLength to totalLength
				((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos(totalLength);

				//if Packet is lastPacket, send one more packet
				if (fileInfo.totalLength - totalLength <= FILE_DATA_SIZE) {
					m_sHeader.fapp_type = DATA_LAST;//change type to DATA_LAST

					sendLength = m_fileDes.Read(filebuf, (fileInfo.totalLength - totalLength));
					m_sHeader.fapp_totlen = sendLength;

					memcpy(m_sHeader.fapp_data, filebuf, sendLength);
					totalLength += sendLength;


					mp_UnderLayer->Send((unsigned char*) &m_sHeader, sendLength + FILE_HEADER_SIZE, FILE_TYPE);
					//change progressbar
					((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos((int) m_fileDes.GetLength());

				}
				else{
					m_sHeader.fapp_type = DATA_MAIN;//packet is not last, type is DATA_MAIN
				}

				if (RecEnd)
					break;
			}
		}
		else {//file size smaller than FILE_MAX_SIZE
			m_sHeader.fapp_type = DATA_LAST;
			m_sHeader.fapp_totlen = m_fileDes.GetLength();
			m_sHeader.fapp_msg_type = MSG_DATA;

			sendLength = m_fileDes.Read(filebuf, m_sHeader.fapp_totlen);

			memcpy(m_sHeader.fapp_data, filebuf, sendLength);
			mp_UnderLayer->Send((unsigned char*) &m_sHeader, sendLength + FILE_HEADER_SIZE, FILE_TYPE);
			((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos((unsigned int) m_fileDes.GetLength());
		}
		m_fileDes.Close();//end file send
		m_ackReady = FALSE;
		BooleanErr = TRUE;
		RecEnd = FALSE;
		AfxMessageBox("파일전송완료!");
	}
	CATCH(CFileException, e)
	{
#ifdef _DEBUGd
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		return FALSE;
	}
	END_CATCH

		return TRUE;
}

unsigned char* CFileLayer::Receive(int Thread_type)
{
	unsigned char* ppayload = mp_UnderLayer->Receive(Thread_type);
	if (ppayload != NULL){
		PFILE_APP fileHead = (PFILE_APP) ppayload;

		if (fileHead->fapp_msg_type == MSG_DATA){//fapp_msg_type == MSG_DATA
			FILE_APP fileApp;

			memset(fileApp.fapp_data, '\0', FILE_DATA_SIZE);
			fileApp.fapp_totlen = 0;
			fileApp.fapp_msg_type = MSG_RESP;///
			fileApp.fapp_seq_num = 0;
			fileApp.fapp_type = FILE_TYPE;
			fileApp.ed = 0;

			if (fileHead->fapp_type == DATA_HEAD) {//set file receiver
				PFILE_APP_INFO fileInfo = (PFILE_APP_INFO) fileHead->fapp_data;
				//init progressbar
				((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetRange32(0, (int) fileInfo->totalLength);
				((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos(0);
				AfxMessageBox("파일전송을 시작합니다");
				char buff [] = "All Files(*.*)|*.*||";

				CFileDialog fd(FALSE, NULL, (const char*) fileInfo->fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, buff);

				if (IDOK == fd.DoModal()) {
					recvPath = fd.GetPathName();
				}

				check = fileHead->fapp_seq_num;//set check's num and seq_num
				//sned RESP packet to Sender		
				m_recvFile.Open((const char*) recvPath, CFile::modeCreate | CFile::modeWrite);
				mp_UnderLayer->Send((unsigned char*) &fileApp, FILE_HEADER_SIZE, FILE_TYPE);
				return 0;
			}//end File Init
			else if (fileHead->fapp_type == MSG_COR){
				BooleanErr = TRUE;
			}
			else if (BooleanErr  && fileHead->fapp_type == DATA_MAIN) {
				//error process
				//send sucess packet seq_num to Sender

				if ((check + 1 != fileHead->fapp_seq_num) && (fileHead->fapp_seq_num != 0))
				{
						//send just one ERR packet to Sender
					if (BooleanErr){
						fileApp.fapp_msg_type = MSG_ERR;//fapp_msg_type == ERR
						fileApp.fapp_seq_num = check+1;
						mp_UnderLayer->Send((unsigned char*) &fileApp, FILE_HEADER_SIZE, FILE_TYPE);
						BooleanErr = FALSE;
					}
				}
				else
				{
					((CIPCAppDlg*) mp_aUpperLayer[0])->m_progress.SetPos(check * fileHead->fapp_totlen);
					check = fileHead->fapp_seq_num;
					m_recvFile.Write(fileHead->fapp_data, fileHead->fapp_totlen);
				}
			}
			else if (BooleanErr && fileHead->fapp_type == DATA_LAST){//fapp_type == DATA_LAST
				if (fileHead->fapp_totlen == 0) 
					return 0;
				
				fileApp.fapp_msg_type = MSG_LASTOK;
				mp_UnderLayer->Send((unsigned char*) &fileApp, FILE_HEADER_SIZE, FILE_TYPE);

				m_recvFile.Write(fileHead->fapp_data, fileHead->fapp_totlen);
				m_recvFile.Close();
				AfxMessageBox("파일전송을 완료했습니다");
			}
			else if (fileHead->fapp_type == MSG_CANCLE){//file cancle
				m_recvFile.Close();
				m_recvFile.Remove((const char*)recvPath);
				AfxMessageBox("파일전송이 취소되었습니다");
			}
		}
		//sender unlock unlimit loop
		else if (fileHead->fapp_msg_type == MSG_RESP) {
			m_ackReady = TRUE;
		}
		//sender do err process and check errReady = 1
		else if (fileHead->fapp_msg_type == MSG_ERR){
			m_sHeader.fapp_seq_num = fileHead->fapp_seq_num;
			errReady = 1;

			m_sHeader.fapp_type = DATA_MAIN;
		}
		else if (fileHead->fapp_msg_type == MSG_LASTOK){
			RecEnd = TRUE;
		}
		return 0;
	}
	return 0;
}