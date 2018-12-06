//
// Generated file, do not edit! Created by nedtool 5.0 from src/KInternalMsg.msg.
//

#ifndef __KINTERNALMSG_M_H
#define __KINTERNALMSG_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0500
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>src/KInternalMsg.msg:10</tt> by nedtool.
 * <pre>
 * // Message to register an application
 * packet KRegisterAppMsg
 * {
 *     string appName;
 *     string prefixName;
 * }
 * </pre>
 */
class KRegisterAppMsg : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string appName;
    ::omnetpp::opp_string prefixName;

  private:
    void copy(const KRegisterAppMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const KRegisterAppMsg&);

  public:
    KRegisterAppMsg(const char *name=nullptr, int kind=0);
    KRegisterAppMsg(const KRegisterAppMsg& other);
    virtual ~KRegisterAppMsg();
    KRegisterAppMsg& operator=(const KRegisterAppMsg& other);
    virtual KRegisterAppMsg *dup() const {return new KRegisterAppMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getAppName() const;
    virtual void setAppName(const char * appName);
    virtual const char * getPrefixName() const;
    virtual void setPrefixName(const char * prefixName);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const KRegisterAppMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, KRegisterAppMsg& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>src/KInternalMsg.msg:16</tt> by nedtool.
 * <pre>
 * // Message to provide the current neighbour list
 * packet KNeighbourListMsg
 * {
 *     string neighbourNameList[];
 *     int neighbourNameCount;
 * }
 * </pre>
 */
class KNeighbourListMsg : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string *neighbourNameList; // array ptr
    unsigned int neighbourNameList_arraysize;
    int neighbourNameCount;

  private:
    void copy(const KNeighbourListMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const KNeighbourListMsg&);

  public:
    KNeighbourListMsg(const char *name=nullptr, int kind=0);
    KNeighbourListMsg(const KNeighbourListMsg& other);
    virtual ~KNeighbourListMsg();
    KNeighbourListMsg& operator=(const KNeighbourListMsg& other);
    virtual KNeighbourListMsg *dup() const {return new KNeighbourListMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual void setNeighbourNameListArraySize(unsigned int size);
    virtual unsigned int getNeighbourNameListArraySize() const;
    virtual const char * getNeighbourNameList(unsigned int k) const;
    virtual void setNeighbourNameList(unsigned int k, const char * neighbourNameList);
    virtual int getNeighbourNameCount() const;
    virtual void setNeighbourNameCount(int neighbourNameCount);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const KNeighbourListMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, KNeighbourListMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __KINTERNALMSG_M_H

