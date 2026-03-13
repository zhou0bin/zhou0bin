#include "diagscriptparsing.h"
#ifdef DIAGSCRIPTPARSING_TEST
class myc : public ScriptProcess::Callback
{
public:
    virtual void MessageCallback(ReceiveData& rd)
    {
        std::cout<<"________class callback start______"<<std::endl;
        std::cout<<rd.guid<<" "<<rd.err_code<<std::endl;
        std::cout<<"________class callback end_______"<<std::endl;
    }
};

void callfunc(ReceiveData& rd)
{
    std::cout<<"________func callback start_______"<<std::endl;
    std::cout<<rd.guid<<" "<<rd.err_code<<std::endl;
    std::cout<<"________func callback end_______"<<std::endl;
}
int main()
{
    
    std::cout << std::endl;
    ScriptProcess* spinstance = ScriptProcess::GetInstance();
    if (!spinstance)
    {
        std::cout << __FUNCTION__ << " not get instance! " << std::endl;
        return -1;
    }
    int err = spinstance->Start("/tmp/asf_ds_socket_pip_file.sock");
    // int err = spinstance->Start("/tmp/asf_ds_socket_pip_file.sock", callfunc);
    // std::shared_ptr<myc> mc  = std::make_shared<myc>();
    // int err = spinstance->Start("/tmp/asf_ds_socket_pip_file.sock", mc);
    if(err == -1)
    {
        std::cout << __FUNCTION__ << " not start! " << std::endl;
        spinstance->Stop();
        return -1;
    }
    std::string guid{"testCCU_new"};
    SendData sdata;
    sdata.itype = kLocal;
    sdata.guid = guid;
    spinstance->Send(&sdata);

    // (void)usleep(1000*1450);
    // ErrorType cod = spinstance->ReceiveDatas(sdata.guid);
    // std::cout<<(int)cod<<std::endl;
    sleep(2);
    sdata.itype = kStop;
    spinstance->Send(&sdata);
    ReceiveData rdata;
    ErrorType dr = spinstance->ReceiveDatas(guid.c_str());
    if (dr != KUnKnown)
    {
        std::cout << " type: " << (int)rdata.itype << " error: " << (int)rdata.err_code << " guid: " << rdata.guid << std::endl;
    }
    spinstance->Stop();
    std::cout << std::endl;

    // while(1);
}
#endif
