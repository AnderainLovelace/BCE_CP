#include "ClassPad.h"
#include <stdlib.h>

char* automatic_balance_ce(char *nptr);

enum BCEMessages
{ 
	BALANCE_ID = 2002,
};


class BCEWindow: public CPModuleWindow
{
 private:
         CPEditBox * bce_in, * bce_out;
         virtual void AddUI();
         SIGNED Message(const PegMessage &Mesg);
 public:
        void Draw();
        BCEWindow(PegRect rect,CPMainFrame * frame);
};

BCEWindow::BCEWindow(PegRect rect,CPMainFrame * frame)
:CPModuleWindow(rect,0,NULL,frame)
{
}

void BCEWindow::Draw()
{
	BeginDraw();
	DrawFrame();
	DrawChildren();
	EndDraw();
};

void BCEWindow::AddUI()
{
    PegRect rr = GetToolbarButtonRect();
    PegTextButton * b = new PegTextButton(0,0,"Balance", BALANCE_ID, AF_ENABLED|TT_COPY);
	m_ui->AddToolbarButton(b);
	
    PegRect r = mReal; 
    CPEditBox *p;
    
    r.wTop = r.wBottom/2; 
    bce_out = p = new CPEditBox(r);
    p->SetScrollMode(WSM_AUTOSCROLL);
    Add(p);
    
    r = mReal;
    r.wBottom = r.wBottom/2; 
    bce_in = p = new CPEditBox(r); 
    p->SetScrollMode(WSM_AUTOSCROLL);
    Add(p);
}

SIGNED BCEWindow::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
  	{
		case SIGNAL(BALANCE_ID, PSF_CLICKED):
		    char * exp_in;
            char * exp_out;
		    exp_in  = bce_in->DataGet();
		    if (exp_in!=NULL && *exp_in!='\0')
                exp_out = automatic_balance_ce(exp_in);
            else
                break;
            if (exp_out==NULL)
            {
                bce_out->DataSet("Error!");
            }
            else
            {
                bce_out->DataSet(exp_out);
                free(exp_out);
            }
            bce_out->Draw();
			break;
		default:								
			return CPModuleWindow::Message(Mesg);
 	}
 	return 0;
}   
 
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create some simple Windows: 
	PegRect Rect;
	Rect.Set(MAINFRAME_LEFT, MAINFRAME_TOP, MAINFRAME_RIGHT, MAINFRAME_BOTTOM);

	CPMainFrame *mw = new CPMainFrame(Rect);

	PegRect ChildRect = mw->FullAppRectangle();
	BCEWindow* win = new BCEWindow(ChildRect,mw);
	mw->SetTopWindow(win);

	// Need to set a main window for this module.  In our case, it is the scribble window
	mw->SetMainWindow(win);
	pPresentation->Add(mw);
};


extern "C" char *ExtensionGetLang(ID_MESSAGE MessageNo)
{
	return "";
}

