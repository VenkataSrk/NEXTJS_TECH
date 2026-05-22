#pragma once
/*----------------------------------------------------------------* 
 * Description: Driver for HTTP server
 * Author:      Junanto 21/02/07
 *----------------------------------------------------------------*/

#include "serviceprovider.h"
#include "cfg.h"

/** Forward declarations
 */
class HTTP_Service_Provider;
class HTTP_Service_API;


/** HTTP_Service_Manager: the singleton entity that manages all HTTP server 
    driver instances
 */
class HTTP_Service_Manager : public ServiceProvider
{
public:
    // public destructor
    virtual ~HTTP_Service_Manager();

    /** Create a singleton instance of HTTP_Service_Manager
     */
    static HTTP_Service_Manager* Create();

    /** Returns the only instance of HTTP_Service_Manager. 
	The service manager must have been instantiated with Create() prior
	to calling this function.
    */
    static HTTP_Service_Manager* Instance();

    /** GetConfigTree: Returns a pointer to the configuration tree of
	interface sub commands
     */
    Cfg_Tree_Group* GetConfigTree();

    void Add_Service_To_List(HTTP_Service_Provider*);
    void Remove_Service_From_List(HTTP_Service_Provider*);

    int Get_TLS() { return _tls; }

private:
    typedef std::list<HTTP_Service_Provider*> HTTP_Service_List;

    Cfg_Tree_Group* _cfgTree;
    static HTTP_Service_Manager* _instance;
    HTTP_Service_List _list;
    int _tls;
    HTTP_Service_API* _api;

    HTTP_Service_Provider* Find_Service_By_Name(const char* name);

    // private constructor
    HTTP_Service_Manager();

    /** Terminal command handler
     */
    void DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Show configuration for the specified entry.
	This function will be called by the framework whenever it needs to display the
	specified entry. Previously this instance should tell the entry that it wishes to
	handle displaying the entry by calling entry->RegisterShowHandler().
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String & output, 
	const String & indent, 
	int indent_size) const;


    /** This function will be called whenever a new task is created.
     */
    bool AttachTask(CTask *);

    /** This function will be called whenever a task is about to be deleted.
     */
    void DetachTask(CTask *);
};
