#include "AskProxy.h"
#include "AskProxyDlg.h"
/*extern CAskProxyDlg theApp;

inline CAskProxyDlg* GetApp()
{
    return &theApp;
}
*/

CAskProxyDlg* GetApp()
{
    static CAskProxyDlg *app = new CAskProxyDlg();
    return app;
}
