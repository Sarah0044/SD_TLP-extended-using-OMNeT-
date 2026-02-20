//
// Generated file, do not edit! Created by opp_msgtool 6.3 from message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(EvUpdate)

EvUpdate::EvUpdate(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

EvUpdate::EvUpdate(const EvUpdate& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

EvUpdate::~EvUpdate()
{
}

EvUpdate& EvUpdate::operator=(const EvUpdate& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void EvUpdate::copy(const EvUpdate& other)
{
    this->evId = other.evId;
    this->severity = other.severity;
    this->targetInter = other.targetInter;
    this->approach = other.approach;
    this->DEMV = other.DEMV;
    this->speed = other.speed;
    this->tSent = other.tSent;
    this->distToAP = other.distToAP;
}

void EvUpdate::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->evId);
    doParsimPacking(b,this->severity);
    doParsimPacking(b,this->targetInter);
    doParsimPacking(b,this->approach);
    doParsimPacking(b,this->DEMV);
    doParsimPacking(b,this->speed);
    doParsimPacking(b,this->tSent);
    doParsimPacking(b,this->distToAP);
}

void EvUpdate::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->evId);
    doParsimUnpacking(b,this->severity);
    doParsimUnpacking(b,this->targetInter);
    doParsimUnpacking(b,this->approach);
    doParsimUnpacking(b,this->DEMV);
    doParsimUnpacking(b,this->speed);
    doParsimUnpacking(b,this->tSent);
    doParsimUnpacking(b,this->distToAP);
}

int EvUpdate::getEvId() const
{
    return this->evId;
}

void EvUpdate::setEvId(int evId)
{
    this->evId = evId;
}

int EvUpdate::getSeverity() const
{
    return this->severity;
}

void EvUpdate::setSeverity(int severity)
{
    this->severity = severity;
}

int EvUpdate::getTargetInter() const
{
    return this->targetInter;
}

void EvUpdate::setTargetInter(int targetInter)
{
    this->targetInter = targetInter;
}

int EvUpdate::getApproach() const
{
    return this->approach;
}

void EvUpdate::setApproach(int approach)
{
    this->approach = approach;
}

double EvUpdate::getDEMV() const
{
    return this->DEMV;
}

void EvUpdate::setDEMV(double DEMV)
{
    this->DEMV = DEMV;
}

double EvUpdate::getSpeed() const
{
    return this->speed;
}

void EvUpdate::setSpeed(double speed)
{
    this->speed = speed;
}

omnetpp::simtime_t EvUpdate::getTSent() const
{
    return this->tSent;
}

void EvUpdate::setTSent(omnetpp::simtime_t tSent)
{
    this->tSent = tSent;
}

double EvUpdate::getDistToAP() const
{
    return this->distToAP;
}

void EvUpdate::setDistToAP(double distToAP)
{
    this->distToAP = distToAP;
}

class EvUpdateDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_evId,
        FIELD_severity,
        FIELD_targetInter,
        FIELD_approach,
        FIELD_DEMV,
        FIELD_speed,
        FIELD_tSent,
        FIELD_distToAP,
    };
  public:
    EvUpdateDescriptor();
    virtual ~EvUpdateDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(EvUpdateDescriptor)

EvUpdateDescriptor::EvUpdateDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EvUpdate)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

EvUpdateDescriptor::~EvUpdateDescriptor()
{
    delete[] propertyNames;
}

bool EvUpdateDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EvUpdate *>(obj)!=nullptr;
}

const char **EvUpdateDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EvUpdateDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EvUpdateDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int EvUpdateDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_evId
        FD_ISEDITABLE,    // FIELD_severity
        FD_ISEDITABLE,    // FIELD_targetInter
        FD_ISEDITABLE,    // FIELD_approach
        FD_ISEDITABLE,    // FIELD_DEMV
        FD_ISEDITABLE,    // FIELD_speed
        FD_ISEDITABLE,    // FIELD_tSent
        FD_ISEDITABLE,    // FIELD_distToAP
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *EvUpdateDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "evId",
        "severity",
        "targetInter",
        "approach",
        "DEMV",
        "speed",
        "tSent",
        "distToAP",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int EvUpdateDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "evId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "severity") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "targetInter") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "approach") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "DEMV") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "speed") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "tSent") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "distToAP") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *EvUpdateDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_evId
        "int",    // FIELD_severity
        "int",    // FIELD_targetInter
        "int",    // FIELD_approach
        "double",    // FIELD_DEMV
        "double",    // FIELD_speed
        "omnetpp::simtime_t",    // FIELD_tSent
        "double",    // FIELD_distToAP
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **EvUpdateDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *EvUpdateDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int EvUpdateDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EvUpdateDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EvUpdate'", field);
    }
}

const char *EvUpdateDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EvUpdateDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return long2string(pp->getEvId());
        case FIELD_severity: return long2string(pp->getSeverity());
        case FIELD_targetInter: return long2string(pp->getTargetInter());
        case FIELD_approach: return long2string(pp->getApproach());
        case FIELD_DEMV: return double2string(pp->getDEMV());
        case FIELD_speed: return double2string(pp->getSpeed());
        case FIELD_tSent: return simtime2string(pp->getTSent());
        case FIELD_distToAP: return double2string(pp->getDistToAP());
        default: return "";
    }
}

void EvUpdateDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(string2long(value)); break;
        case FIELD_severity: pp->setSeverity(string2long(value)); break;
        case FIELD_targetInter: pp->setTargetInter(string2long(value)); break;
        case FIELD_approach: pp->setApproach(string2long(value)); break;
        case FIELD_DEMV: pp->setDEMV(string2double(value)); break;
        case FIELD_speed: pp->setSpeed(string2double(value)); break;
        case FIELD_tSent: pp->setTSent(string2simtime(value)); break;
        case FIELD_distToAP: pp->setDistToAP(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvUpdate'", field);
    }
}

omnetpp::cValue EvUpdateDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return pp->getEvId();
        case FIELD_severity: return pp->getSeverity();
        case FIELD_targetInter: return pp->getTargetInter();
        case FIELD_approach: return pp->getApproach();
        case FIELD_DEMV: return pp->getDEMV();
        case FIELD_speed: return pp->getSpeed();
        case FIELD_tSent: return pp->getTSent().dbl();
        case FIELD_distToAP: return pp->getDistToAP();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EvUpdate' as cValue -- field index out of range?", field);
    }
}

void EvUpdateDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_severity: pp->setSeverity(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_targetInter: pp->setTargetInter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_approach: pp->setApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_DEMV: pp->setDEMV(value.doubleValue()); break;
        case FIELD_speed: pp->setSpeed(value.doubleValue()); break;
        case FIELD_tSent: pp->setTSent(value.doubleValue()); break;
        case FIELD_distToAP: pp->setDistToAP(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvUpdate'", field);
    }
}

const char *EvUpdateDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr EvUpdateDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EvUpdateDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EvUpdate *pp = omnetpp::fromAnyPtr<EvUpdate>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvUpdate'", field);
    }
}

Register_Class(TlCommand)

TlCommand::TlCommand(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

TlCommand::TlCommand(const TlCommand& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

TlCommand::~TlCommand()
{
}

TlCommand& TlCommand::operator=(const TlCommand& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void TlCommand::copy(const TlCommand& other)
{
    this->intersectionId = other.intersectionId;
    this->approach = other.approach;
    this->action = other.action;
    this->duration = other.duration;
}

void TlCommand::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->intersectionId);
    doParsimPacking(b,this->approach);
    doParsimPacking(b,this->action);
    doParsimPacking(b,this->duration);
}

void TlCommand::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->intersectionId);
    doParsimUnpacking(b,this->approach);
    doParsimUnpacking(b,this->action);
    doParsimUnpacking(b,this->duration);
}

int TlCommand::getIntersectionId() const
{
    return this->intersectionId;
}

void TlCommand::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

int TlCommand::getApproach() const
{
    return this->approach;
}

void TlCommand::setApproach(int approach)
{
    this->approach = approach;
}

const char * TlCommand::getAction() const
{
    return this->action.c_str();
}

void TlCommand::setAction(const char * action)
{
    this->action = action;
}

double TlCommand::getDuration() const
{
    return this->duration;
}

void TlCommand::setDuration(double duration)
{
    this->duration = duration;
}

class TlCommandDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_intersectionId,
        FIELD_approach,
        FIELD_action,
        FIELD_duration,
    };
  public:
    TlCommandDescriptor();
    virtual ~TlCommandDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TlCommandDescriptor)

TlCommandDescriptor::TlCommandDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TlCommand)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

TlCommandDescriptor::~TlCommandDescriptor()
{
    delete[] propertyNames;
}

bool TlCommandDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TlCommand *>(obj)!=nullptr;
}

const char **TlCommandDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TlCommandDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TlCommandDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int TlCommandDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_intersectionId
        FD_ISEDITABLE,    // FIELD_approach
        FD_ISEDITABLE,    // FIELD_action
        FD_ISEDITABLE,    // FIELD_duration
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *TlCommandDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "intersectionId",
        "approach",
        "action",
        "duration",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int TlCommandDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "approach") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "action") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "duration") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *TlCommandDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_intersectionId
        "int",    // FIELD_approach
        "string",    // FIELD_action
        "double",    // FIELD_duration
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **TlCommandDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TlCommandDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TlCommandDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TlCommandDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TlCommand'", field);
    }
}

const char *TlCommandDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TlCommandDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        case FIELD_approach: return long2string(pp->getApproach());
        case FIELD_action: return oppstring2string(pp->getAction());
        case FIELD_duration: return double2string(pp->getDuration());
        default: return "";
    }
}

void TlCommandDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        case FIELD_approach: pp->setApproach(string2long(value)); break;
        case FIELD_action: pp->setAction((value)); break;
        case FIELD_duration: pp->setDuration(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TlCommand'", field);
    }
}

omnetpp::cValue TlCommandDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return pp->getIntersectionId();
        case FIELD_approach: return pp->getApproach();
        case FIELD_action: return pp->getAction();
        case FIELD_duration: return pp->getDuration();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TlCommand' as cValue -- field index out of range?", field);
    }
}

void TlCommandDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_approach: pp->setApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_action: pp->setAction(value.stringValue()); break;
        case FIELD_duration: pp->setDuration(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TlCommand'", field);
    }
}

const char *TlCommandDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TlCommandDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TlCommandDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TlCommand *pp = omnetpp::fromAnyPtr<TlCommand>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TlCommand'", field);
    }
}

Register_Class(QueueReport)

QueueReport::QueueReport(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

QueueReport::QueueReport(const QueueReport& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

QueueReport::~QueueReport()
{
}

QueueReport& QueueReport::operator=(const QueueReport& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void QueueReport::copy(const QueueReport& other)
{
    this->intersectionId = other.intersectionId;
    this->approach = other.approach;
    this->C = other.C;
    this->TD = other.TD;
}

void QueueReport::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->intersectionId);
    doParsimPacking(b,this->approach);
    doParsimPacking(b,this->C);
    doParsimPacking(b,this->TD);
}

void QueueReport::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->intersectionId);
    doParsimUnpacking(b,this->approach);
    doParsimUnpacking(b,this->C);
    doParsimUnpacking(b,this->TD);
}

int QueueReport::getIntersectionId() const
{
    return this->intersectionId;
}

void QueueReport::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

int QueueReport::getApproach() const
{
    return this->approach;
}

void QueueReport::setApproach(int approach)
{
    this->approach = approach;
}

int QueueReport::getC() const
{
    return this->C;
}

void QueueReport::setC(int C)
{
    this->C = C;
}

double QueueReport::getTD() const
{
    return this->TD;
}

void QueueReport::setTD(double TD)
{
    this->TD = TD;
}

class QueueReportDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_intersectionId,
        FIELD_approach,
        FIELD_C,
        FIELD_TD,
    };
  public:
    QueueReportDescriptor();
    virtual ~QueueReportDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(QueueReportDescriptor)

QueueReportDescriptor::QueueReportDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(QueueReport)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

QueueReportDescriptor::~QueueReportDescriptor()
{
    delete[] propertyNames;
}

bool QueueReportDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<QueueReport *>(obj)!=nullptr;
}

const char **QueueReportDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *QueueReportDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int QueueReportDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int QueueReportDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_intersectionId
        FD_ISEDITABLE,    // FIELD_approach
        FD_ISEDITABLE,    // FIELD_C
        FD_ISEDITABLE,    // FIELD_TD
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *QueueReportDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "intersectionId",
        "approach",
        "C",
        "TD",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int QueueReportDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "approach") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "C") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "TD") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *QueueReportDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_intersectionId
        "int",    // FIELD_approach
        "int",    // FIELD_C
        "double",    // FIELD_TD
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **QueueReportDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *QueueReportDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int QueueReportDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void QueueReportDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'QueueReport'", field);
    }
}

const char *QueueReportDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string QueueReportDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        case FIELD_approach: return long2string(pp->getApproach());
        case FIELD_C: return long2string(pp->getC());
        case FIELD_TD: return double2string(pp->getTD());
        default: return "";
    }
}

void QueueReportDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        case FIELD_approach: pp->setApproach(string2long(value)); break;
        case FIELD_C: pp->setC(string2long(value)); break;
        case FIELD_TD: pp->setTD(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'QueueReport'", field);
    }
}

omnetpp::cValue QueueReportDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return pp->getIntersectionId();
        case FIELD_approach: return pp->getApproach();
        case FIELD_C: return pp->getC();
        case FIELD_TD: return pp->getTD();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'QueueReport' as cValue -- field index out of range?", field);
    }
}

void QueueReportDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_approach: pp->setApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_C: pp->setC(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_TD: pp->setTD(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'QueueReport'", field);
    }
}

const char *QueueReportDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr QueueReportDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void QueueReportDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    QueueReport *pp = omnetpp::fromAnyPtr<QueueReport>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'QueueReport'", field);
    }
}

Register_Class(SignalState)

SignalState::SignalState(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

SignalState::SignalState(const SignalState& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

SignalState::~SignalState()
{
}

SignalState& SignalState::operator=(const SignalState& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void SignalState::copy(const SignalState& other)
{
    this->intersectionId = other.intersectionId;
    this->greenApproach = other.greenApproach;
    this->preemptActive = other.preemptActive;
}

void SignalState::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->intersectionId);
    doParsimPacking(b,this->greenApproach);
    doParsimPacking(b,this->preemptActive);
}

void SignalState::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->intersectionId);
    doParsimUnpacking(b,this->greenApproach);
    doParsimUnpacking(b,this->preemptActive);
}

int SignalState::getIntersectionId() const
{
    return this->intersectionId;
}

void SignalState::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

int SignalState::getGreenApproach() const
{
    return this->greenApproach;
}

void SignalState::setGreenApproach(int greenApproach)
{
    this->greenApproach = greenApproach;
}

bool SignalState::getPreemptActive() const
{
    return this->preemptActive;
}

void SignalState::setPreemptActive(bool preemptActive)
{
    this->preemptActive = preemptActive;
}

class SignalStateDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_intersectionId,
        FIELD_greenApproach,
        FIELD_preemptActive,
    };
  public:
    SignalStateDescriptor();
    virtual ~SignalStateDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(SignalStateDescriptor)

SignalStateDescriptor::SignalStateDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(SignalState)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

SignalStateDescriptor::~SignalStateDescriptor()
{
    delete[] propertyNames;
}

bool SignalStateDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SignalState *>(obj)!=nullptr;
}

const char **SignalStateDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *SignalStateDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int SignalStateDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int SignalStateDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_intersectionId
        FD_ISEDITABLE,    // FIELD_greenApproach
        FD_ISEDITABLE,    // FIELD_preemptActive
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *SignalStateDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "intersectionId",
        "greenApproach",
        "preemptActive",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int SignalStateDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "greenApproach") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "preemptActive") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *SignalStateDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_intersectionId
        "int",    // FIELD_greenApproach
        "bool",    // FIELD_preemptActive
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **SignalStateDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *SignalStateDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int SignalStateDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void SignalStateDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'SignalState'", field);
    }
}

const char *SignalStateDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SignalStateDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        case FIELD_greenApproach: return long2string(pp->getGreenApproach());
        case FIELD_preemptActive: return bool2string(pp->getPreemptActive());
        default: return "";
    }
}

void SignalStateDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        case FIELD_greenApproach: pp->setGreenApproach(string2long(value)); break;
        case FIELD_preemptActive: pp->setPreemptActive(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SignalState'", field);
    }
}

omnetpp::cValue SignalStateDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: return pp->getIntersectionId();
        case FIELD_greenApproach: return pp->getGreenApproach();
        case FIELD_preemptActive: return pp->getPreemptActive();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'SignalState' as cValue -- field index out of range?", field);
    }
}

void SignalStateDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_greenApproach: pp->setGreenApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_preemptActive: pp->setPreemptActive(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SignalState'", field);
    }
}

const char *SignalStateDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr SignalStateDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void SignalStateDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    SignalState *pp = omnetpp::fromAnyPtr<SignalState>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SignalState'", field);
    }
}

Register_Class(EvAtStopLine)

EvAtStopLine::EvAtStopLine(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

EvAtStopLine::EvAtStopLine(const EvAtStopLine& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

EvAtStopLine::~EvAtStopLine()
{
}

EvAtStopLine& EvAtStopLine::operator=(const EvAtStopLine& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void EvAtStopLine::copy(const EvAtStopLine& other)
{
    this->evId = other.evId;
    this->intersectionId = other.intersectionId;
    this->approach = other.approach;
    this->severity = other.severity;
}

void EvAtStopLine::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->evId);
    doParsimPacking(b,this->intersectionId);
    doParsimPacking(b,this->approach);
    doParsimPacking(b,this->severity);
}

void EvAtStopLine::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->evId);
    doParsimUnpacking(b,this->intersectionId);
    doParsimUnpacking(b,this->approach);
    doParsimUnpacking(b,this->severity);
}

int EvAtStopLine::getEvId() const
{
    return this->evId;
}

void EvAtStopLine::setEvId(int evId)
{
    this->evId = evId;
}

int EvAtStopLine::getIntersectionId() const
{
    return this->intersectionId;
}

void EvAtStopLine::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

int EvAtStopLine::getApproach() const
{
    return this->approach;
}

void EvAtStopLine::setApproach(int approach)
{
    this->approach = approach;
}

int EvAtStopLine::getSeverity() const
{
    return this->severity;
}

void EvAtStopLine::setSeverity(int severity)
{
    this->severity = severity;
}

class EvAtStopLineDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_evId,
        FIELD_intersectionId,
        FIELD_approach,
        FIELD_severity,
    };
  public:
    EvAtStopLineDescriptor();
    virtual ~EvAtStopLineDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(EvAtStopLineDescriptor)

EvAtStopLineDescriptor::EvAtStopLineDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EvAtStopLine)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

EvAtStopLineDescriptor::~EvAtStopLineDescriptor()
{
    delete[] propertyNames;
}

bool EvAtStopLineDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EvAtStopLine *>(obj)!=nullptr;
}

const char **EvAtStopLineDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EvAtStopLineDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EvAtStopLineDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int EvAtStopLineDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_evId
        FD_ISEDITABLE,    // FIELD_intersectionId
        FD_ISEDITABLE,    // FIELD_approach
        FD_ISEDITABLE,    // FIELD_severity
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *EvAtStopLineDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "evId",
        "intersectionId",
        "approach",
        "severity",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int EvAtStopLineDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "evId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "approach") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "severity") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *EvAtStopLineDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_evId
        "int",    // FIELD_intersectionId
        "int",    // FIELD_approach
        "int",    // FIELD_severity
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **EvAtStopLineDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *EvAtStopLineDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int EvAtStopLineDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EvAtStopLineDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EvAtStopLine'", field);
    }
}

const char *EvAtStopLineDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EvAtStopLineDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return long2string(pp->getEvId());
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        case FIELD_approach: return long2string(pp->getApproach());
        case FIELD_severity: return long2string(pp->getSeverity());
        default: return "";
    }
}

void EvAtStopLineDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(string2long(value)); break;
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        case FIELD_approach: pp->setApproach(string2long(value)); break;
        case FIELD_severity: pp->setSeverity(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvAtStopLine'", field);
    }
}

omnetpp::cValue EvAtStopLineDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return pp->getEvId();
        case FIELD_intersectionId: return pp->getIntersectionId();
        case FIELD_approach: return pp->getApproach();
        case FIELD_severity: return pp->getSeverity();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EvAtStopLine' as cValue -- field index out of range?", field);
    }
}

void EvAtStopLineDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_approach: pp->setApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_severity: pp->setSeverity(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvAtStopLine'", field);
    }
}

const char *EvAtStopLineDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr EvAtStopLineDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EvAtStopLineDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EvAtStopLine *pp = omnetpp::fromAnyPtr<EvAtStopLine>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvAtStopLine'", field);
    }
}

Register_Class(EvGo)

EvGo::EvGo(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

EvGo::EvGo(const EvGo& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

EvGo::~EvGo()
{
}

EvGo& EvGo::operator=(const EvGo& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void EvGo::copy(const EvGo& other)
{
    this->evId = other.evId;
    this->intersectionId = other.intersectionId;
}

void EvGo::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->evId);
    doParsimPacking(b,this->intersectionId);
}

void EvGo::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->evId);
    doParsimUnpacking(b,this->intersectionId);
}

int EvGo::getEvId() const
{
    return this->evId;
}

void EvGo::setEvId(int evId)
{
    this->evId = evId;
}

int EvGo::getIntersectionId() const
{
    return this->intersectionId;
}

void EvGo::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

class EvGoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_evId,
        FIELD_intersectionId,
    };
  public:
    EvGoDescriptor();
    virtual ~EvGoDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(EvGoDescriptor)

EvGoDescriptor::EvGoDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EvGo)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

EvGoDescriptor::~EvGoDescriptor()
{
    delete[] propertyNames;
}

bool EvGoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EvGo *>(obj)!=nullptr;
}

const char **EvGoDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EvGoDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EvGoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int EvGoDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_evId
        FD_ISEDITABLE,    // FIELD_intersectionId
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *EvGoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "evId",
        "intersectionId",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int EvGoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "evId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *EvGoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_evId
        "int",    // FIELD_intersectionId
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **EvGoDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *EvGoDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int EvGoDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EvGoDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EvGo'", field);
    }
}

const char *EvGoDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EvGoDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return long2string(pp->getEvId());
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        default: return "";
    }
}

void EvGoDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(string2long(value)); break;
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvGo'", field);
    }
}

omnetpp::cValue EvGoDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return pp->getEvId();
        case FIELD_intersectionId: return pp->getIntersectionId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EvGo' as cValue -- field index out of range?", field);
    }
}

void EvGoDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvGo'", field);
    }
}

const char *EvGoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr EvGoDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EvGoDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EvGo *pp = omnetpp::fromAnyPtr<EvGo>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvGo'", field);
    }
}

Register_Class(EvCleared)

EvCleared::EvCleared(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

EvCleared::EvCleared(const EvCleared& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

EvCleared::~EvCleared()
{
}

EvCleared& EvCleared::operator=(const EvCleared& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void EvCleared::copy(const EvCleared& other)
{
    this->evId = other.evId;
    this->intersectionId = other.intersectionId;
}

void EvCleared::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->evId);
    doParsimPacking(b,this->intersectionId);
}

void EvCleared::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->evId);
    doParsimUnpacking(b,this->intersectionId);
}

int EvCleared::getEvId() const
{
    return this->evId;
}

void EvCleared::setEvId(int evId)
{
    this->evId = evId;
}

int EvCleared::getIntersectionId() const
{
    return this->intersectionId;
}

void EvCleared::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

class EvClearedDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_evId,
        FIELD_intersectionId,
    };
  public:
    EvClearedDescriptor();
    virtual ~EvClearedDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(EvClearedDescriptor)

EvClearedDescriptor::EvClearedDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EvCleared)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

EvClearedDescriptor::~EvClearedDescriptor()
{
    delete[] propertyNames;
}

bool EvClearedDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EvCleared *>(obj)!=nullptr;
}

const char **EvClearedDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EvClearedDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EvClearedDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int EvClearedDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_evId
        FD_ISEDITABLE,    // FIELD_intersectionId
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *EvClearedDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "evId",
        "intersectionId",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int EvClearedDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "evId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *EvClearedDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_evId
        "int",    // FIELD_intersectionId
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **EvClearedDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *EvClearedDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int EvClearedDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EvClearedDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EvCleared'", field);
    }
}

const char *EvClearedDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EvClearedDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return long2string(pp->getEvId());
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        default: return "";
    }
}

void EvClearedDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(string2long(value)); break;
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvCleared'", field);
    }
}

omnetpp::cValue EvClearedDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return pp->getEvId();
        case FIELD_intersectionId: return pp->getIntersectionId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EvCleared' as cValue -- field index out of range?", field);
    }
}

void EvClearedDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvCleared'", field);
    }
}

const char *EvClearedDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr EvClearedDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EvClearedDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EvCleared *pp = omnetpp::fromAnyPtr<EvCleared>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvCleared'", field);
    }
}

Register_Class(EvQueueAhead)

EvQueueAhead::EvQueueAhead(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

EvQueueAhead::EvQueueAhead(const EvQueueAhead& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

EvQueueAhead::~EvQueueAhead()
{
}

EvQueueAhead& EvQueueAhead::operator=(const EvQueueAhead& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void EvQueueAhead::copy(const EvQueueAhead& other)
{
    this->evId = other.evId;
    this->intersectionId = other.intersectionId;
    this->approach = other.approach;
    this->ahead = other.ahead;
}

void EvQueueAhead::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->evId);
    doParsimPacking(b,this->intersectionId);
    doParsimPacking(b,this->approach);
    doParsimPacking(b,this->ahead);
}

void EvQueueAhead::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->evId);
    doParsimUnpacking(b,this->intersectionId);
    doParsimUnpacking(b,this->approach);
    doParsimUnpacking(b,this->ahead);
}

int EvQueueAhead::getEvId() const
{
    return this->evId;
}

void EvQueueAhead::setEvId(int evId)
{
    this->evId = evId;
}

int EvQueueAhead::getIntersectionId() const
{
    return this->intersectionId;
}

void EvQueueAhead::setIntersectionId(int intersectionId)
{
    this->intersectionId = intersectionId;
}

int EvQueueAhead::getApproach() const
{
    return this->approach;
}

void EvQueueAhead::setApproach(int approach)
{
    this->approach = approach;
}

int EvQueueAhead::getAhead() const
{
    return this->ahead;
}

void EvQueueAhead::setAhead(int ahead)
{
    this->ahead = ahead;
}

class EvQueueAheadDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_evId,
        FIELD_intersectionId,
        FIELD_approach,
        FIELD_ahead,
    };
  public:
    EvQueueAheadDescriptor();
    virtual ~EvQueueAheadDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(EvQueueAheadDescriptor)

EvQueueAheadDescriptor::EvQueueAheadDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EvQueueAhead)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

EvQueueAheadDescriptor::~EvQueueAheadDescriptor()
{
    delete[] propertyNames;
}

bool EvQueueAheadDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EvQueueAhead *>(obj)!=nullptr;
}

const char **EvQueueAheadDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EvQueueAheadDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EvQueueAheadDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int EvQueueAheadDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_evId
        FD_ISEDITABLE,    // FIELD_intersectionId
        FD_ISEDITABLE,    // FIELD_approach
        FD_ISEDITABLE,    // FIELD_ahead
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *EvQueueAheadDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "evId",
        "intersectionId",
        "approach",
        "ahead",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int EvQueueAheadDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "evId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "intersectionId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "approach") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "ahead") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *EvQueueAheadDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_evId
        "int",    // FIELD_intersectionId
        "int",    // FIELD_approach
        "int",    // FIELD_ahead
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **EvQueueAheadDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *EvQueueAheadDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int EvQueueAheadDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EvQueueAheadDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EvQueueAhead'", field);
    }
}

const char *EvQueueAheadDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EvQueueAheadDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return long2string(pp->getEvId());
        case FIELD_intersectionId: return long2string(pp->getIntersectionId());
        case FIELD_approach: return long2string(pp->getApproach());
        case FIELD_ahead: return long2string(pp->getAhead());
        default: return "";
    }
}

void EvQueueAheadDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(string2long(value)); break;
        case FIELD_intersectionId: pp->setIntersectionId(string2long(value)); break;
        case FIELD_approach: pp->setApproach(string2long(value)); break;
        case FIELD_ahead: pp->setAhead(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvQueueAhead'", field);
    }
}

omnetpp::cValue EvQueueAheadDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        case FIELD_evId: return pp->getEvId();
        case FIELD_intersectionId: return pp->getIntersectionId();
        case FIELD_approach: return pp->getApproach();
        case FIELD_ahead: return pp->getAhead();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EvQueueAhead' as cValue -- field index out of range?", field);
    }
}

void EvQueueAheadDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        case FIELD_evId: pp->setEvId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_intersectionId: pp->setIntersectionId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_approach: pp->setApproach(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ahead: pp->setAhead(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvQueueAhead'", field);
    }
}

const char *EvQueueAheadDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr EvQueueAheadDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EvQueueAheadDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EvQueueAhead *pp = omnetpp::fromAnyPtr<EvQueueAhead>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EvQueueAhead'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

