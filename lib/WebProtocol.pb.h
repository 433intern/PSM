// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: WebProtocol.proto

#ifndef PROTOBUF_WebProtocol_2eproto__INCLUDED
#define PROTOBUF_WebProtocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace psmweb {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_WebProtocol_2eproto();
void protobuf_AssignDesc_WebProtocol_2eproto();
void protobuf_ShutdownFile_WebProtocol_2eproto();

class wsProcessCommandRequest;
class swProcessCommandResponse;
class wsHealthCheck;
class swHealthAck;

enum swProcessCommandResponse_Result {
  swProcessCommandResponse_Result_SUCCESS = 0,
  swProcessCommandResponse_Result_FAILURE = 1
};
bool swProcessCommandResponse_Result_IsValid(int value);
const swProcessCommandResponse_Result swProcessCommandResponse_Result_Result_MIN = swProcessCommandResponse_Result_SUCCESS;
const swProcessCommandResponse_Result swProcessCommandResponse_Result_Result_MAX = swProcessCommandResponse_Result_FAILURE;
const int swProcessCommandResponse_Result_Result_ARRAYSIZE = swProcessCommandResponse_Result_Result_MAX + 1;

const ::google::protobuf::EnumDescriptor* swProcessCommandResponse_Result_descriptor();
inline const ::std::string& swProcessCommandResponse_Result_Name(swProcessCommandResponse_Result value) {
  return ::google::protobuf::internal::NameOfEnum(
    swProcessCommandResponse_Result_descriptor(), value);
}
inline bool swProcessCommandResponse_Result_Parse(
    const ::std::string& name, swProcessCommandResponse_Result* value) {
  return ::google::protobuf::internal::ParseNamedEnum<swProcessCommandResponse_Result>(
    swProcessCommandResponse_Result_descriptor(), name, value);
}
enum psmType {
  ProcessCommandRequest = 0,
  ProcessCommandResponse = 1,
  HealthCheck = 8,
  HealthAck = 9
};
bool psmType_IsValid(int value);
const psmType psmType_MIN = ProcessCommandRequest;
const psmType psmType_MAX = HealthAck;
const int psmType_ARRAYSIZE = psmType_MAX + 1;

const ::google::protobuf::EnumDescriptor* psmType_descriptor();
inline const ::std::string& psmType_Name(psmType value) {
  return ::google::protobuf::internal::NameOfEnum(
    psmType_descriptor(), value);
}
inline bool psmType_Parse(
    const ::std::string& name, psmType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<psmType>(
    psmType_descriptor(), name, value);
}
enum ProcessCommandType {
  START = 0,
  RESTART = 1,
  STOP = 2,
  ADDLIST = 3,
  DELETELIST = 4,
  ALLSTOP = 5
};
bool ProcessCommandType_IsValid(int value);
const ProcessCommandType ProcessCommandType_MIN = START;
const ProcessCommandType ProcessCommandType_MAX = ALLSTOP;
const int ProcessCommandType_ARRAYSIZE = ProcessCommandType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProcessCommandType_descriptor();
inline const ::std::string& ProcessCommandType_Name(ProcessCommandType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProcessCommandType_descriptor(), value);
}
inline bool ProcessCommandType_Parse(
    const ::std::string& name, ProcessCommandType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProcessCommandType>(
    ProcessCommandType_descriptor(), name, value);
}
// ===================================================================

class wsProcessCommandRequest : public ::google::protobuf::Message {
 public:
  wsProcessCommandRequest();
  virtual ~wsProcessCommandRequest();

  wsProcessCommandRequest(const wsProcessCommandRequest& from);

  inline wsProcessCommandRequest& operator=(const wsProcessCommandRequest& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const wsProcessCommandRequest& default_instance();

  void Swap(wsProcessCommandRequest* other);

  // implements Message ----------------------------------------------

  wsProcessCommandRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const wsProcessCommandRequest& from);
  void MergeFrom(const wsProcessCommandRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 token = 1;
  inline bool has_token() const;
  inline void clear_token();
  static const int kTokenFieldNumber = 1;
  inline ::google::protobuf::int32 token() const;
  inline void set_token(::google::protobuf::int32 value);

  // required .psmweb.ProcessCommandType type = 2;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 2;
  inline ::psmweb::ProcessCommandType type() const;
  inline void set_type(::psmweb::ProcessCommandType value);

  // required string processName = 3;
  inline bool has_processname() const;
  inline void clear_processname();
  static const int kProcessNameFieldNumber = 3;
  inline const ::std::string& processname() const;
  inline void set_processname(const ::std::string& value);
  inline void set_processname(const char* value);
  inline void set_processname(const char* value, size_t size);
  inline ::std::string* mutable_processname();
  inline ::std::string* release_processname();
  inline void set_allocated_processname(::std::string* processname);

  // @@protoc_insertion_point(class_scope:psmweb.wsProcessCommandRequest)
 private:
  inline void set_has_token();
  inline void clear_has_token();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_processname();
  inline void clear_has_processname();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 token_;
  int type_;
  ::std::string* processname_;
  friend void  protobuf_AddDesc_WebProtocol_2eproto();
  friend void protobuf_AssignDesc_WebProtocol_2eproto();
  friend void protobuf_ShutdownFile_WebProtocol_2eproto();

  void InitAsDefaultInstance();
  static wsProcessCommandRequest* default_instance_;
};
// -------------------------------------------------------------------

class swProcessCommandResponse : public ::google::protobuf::Message {
 public:
  swProcessCommandResponse();
  virtual ~swProcessCommandResponse();

  swProcessCommandResponse(const swProcessCommandResponse& from);

  inline swProcessCommandResponse& operator=(const swProcessCommandResponse& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const swProcessCommandResponse& default_instance();

  void Swap(swProcessCommandResponse* other);

  // implements Message ----------------------------------------------

  swProcessCommandResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const swProcessCommandResponse& from);
  void MergeFrom(const swProcessCommandResponse& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef swProcessCommandResponse_Result Result;
  static const Result SUCCESS = swProcessCommandResponse_Result_SUCCESS;
  static const Result FAILURE = swProcessCommandResponse_Result_FAILURE;
  static inline bool Result_IsValid(int value) {
    return swProcessCommandResponse_Result_IsValid(value);
  }
  static const Result Result_MIN =
    swProcessCommandResponse_Result_Result_MIN;
  static const Result Result_MAX =
    swProcessCommandResponse_Result_Result_MAX;
  static const int Result_ARRAYSIZE =
    swProcessCommandResponse_Result_Result_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Result_descriptor() {
    return swProcessCommandResponse_Result_descriptor();
  }
  static inline const ::std::string& Result_Name(Result value) {
    return swProcessCommandResponse_Result_Name(value);
  }
  static inline bool Result_Parse(const ::std::string& name,
      Result* value) {
    return swProcessCommandResponse_Result_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required int32 token = 1;
  inline bool has_token() const;
  inline void clear_token();
  static const int kTokenFieldNumber = 1;
  inline ::google::protobuf::int32 token() const;
  inline void set_token(::google::protobuf::int32 value);

  // required .psmweb.ProcessCommandType type = 2;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 2;
  inline ::psmweb::ProcessCommandType type() const;
  inline void set_type(::psmweb::ProcessCommandType value);

  // required .psmweb.swProcessCommandResponse.Result result = 3;
  inline bool has_result() const;
  inline void clear_result();
  static const int kResultFieldNumber = 3;
  inline ::psmweb::swProcessCommandResponse_Result result() const;
  inline void set_result(::psmweb::swProcessCommandResponse_Result value);

  // optional string failReason = 4;
  inline bool has_failreason() const;
  inline void clear_failreason();
  static const int kFailReasonFieldNumber = 4;
  inline const ::std::string& failreason() const;
  inline void set_failreason(const ::std::string& value);
  inline void set_failreason(const char* value);
  inline void set_failreason(const char* value, size_t size);
  inline ::std::string* mutable_failreason();
  inline ::std::string* release_failreason();
  inline void set_allocated_failreason(::std::string* failreason);

  // @@protoc_insertion_point(class_scope:psmweb.swProcessCommandResponse)
 private:
  inline void set_has_token();
  inline void clear_has_token();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_result();
  inline void clear_has_result();
  inline void set_has_failreason();
  inline void clear_has_failreason();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 token_;
  int type_;
  ::std::string* failreason_;
  int result_;
  friend void  protobuf_AddDesc_WebProtocol_2eproto();
  friend void protobuf_AssignDesc_WebProtocol_2eproto();
  friend void protobuf_ShutdownFile_WebProtocol_2eproto();

  void InitAsDefaultInstance();
  static swProcessCommandResponse* default_instance_;
};
// -------------------------------------------------------------------

class wsHealthCheck : public ::google::protobuf::Message {
 public:
  wsHealthCheck();
  virtual ~wsHealthCheck();

  wsHealthCheck(const wsHealthCheck& from);

  inline wsHealthCheck& operator=(const wsHealthCheck& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const wsHealthCheck& default_instance();

  void Swap(wsHealthCheck* other);

  // implements Message ----------------------------------------------

  wsHealthCheck* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const wsHealthCheck& from);
  void MergeFrom(const wsHealthCheck& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:psmweb.wsHealthCheck)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_WebProtocol_2eproto();
  friend void protobuf_AssignDesc_WebProtocol_2eproto();
  friend void protobuf_ShutdownFile_WebProtocol_2eproto();

  void InitAsDefaultInstance();
  static wsHealthCheck* default_instance_;
};
// -------------------------------------------------------------------

class swHealthAck : public ::google::protobuf::Message {
 public:
  swHealthAck();
  virtual ~swHealthAck();

  swHealthAck(const swHealthAck& from);

  inline swHealthAck& operator=(const swHealthAck& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const swHealthAck& default_instance();

  void Swap(swHealthAck* other);

  // implements Message ----------------------------------------------

  swHealthAck* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const swHealthAck& from);
  void MergeFrom(const swHealthAck& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:psmweb.swHealthAck)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_WebProtocol_2eproto();
  friend void protobuf_AssignDesc_WebProtocol_2eproto();
  friend void protobuf_ShutdownFile_WebProtocol_2eproto();

  void InitAsDefaultInstance();
  static swHealthAck* default_instance_;
};
// ===================================================================


// ===================================================================

// wsProcessCommandRequest

// required int32 token = 1;
inline bool wsProcessCommandRequest::has_token() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void wsProcessCommandRequest::set_has_token() {
  _has_bits_[0] |= 0x00000001u;
}
inline void wsProcessCommandRequest::clear_has_token() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void wsProcessCommandRequest::clear_token() {
  token_ = 0;
  clear_has_token();
}
inline ::google::protobuf::int32 wsProcessCommandRequest::token() const {
  // @@protoc_insertion_point(field_get:psmweb.wsProcessCommandRequest.token)
  return token_;
}
inline void wsProcessCommandRequest::set_token(::google::protobuf::int32 value) {
  set_has_token();
  token_ = value;
  // @@protoc_insertion_point(field_set:psmweb.wsProcessCommandRequest.token)
}

// required .psmweb.ProcessCommandType type = 2;
inline bool wsProcessCommandRequest::has_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void wsProcessCommandRequest::set_has_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void wsProcessCommandRequest::clear_has_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void wsProcessCommandRequest::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::psmweb::ProcessCommandType wsProcessCommandRequest::type() const {
  // @@protoc_insertion_point(field_get:psmweb.wsProcessCommandRequest.type)
  return static_cast< ::psmweb::ProcessCommandType >(type_);
}
inline void wsProcessCommandRequest::set_type(::psmweb::ProcessCommandType value) {
  assert(::psmweb::ProcessCommandType_IsValid(value));
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:psmweb.wsProcessCommandRequest.type)
}

// required string processName = 3;
inline bool wsProcessCommandRequest::has_processname() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void wsProcessCommandRequest::set_has_processname() {
  _has_bits_[0] |= 0x00000004u;
}
inline void wsProcessCommandRequest::clear_has_processname() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void wsProcessCommandRequest::clear_processname() {
  if (processname_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    processname_->clear();
  }
  clear_has_processname();
}
inline const ::std::string& wsProcessCommandRequest::processname() const {
  // @@protoc_insertion_point(field_get:psmweb.wsProcessCommandRequest.processName)
  return *processname_;
}
inline void wsProcessCommandRequest::set_processname(const ::std::string& value) {
  set_has_processname();
  if (processname_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    processname_ = new ::std::string;
  }
  processname_->assign(value);
  // @@protoc_insertion_point(field_set:psmweb.wsProcessCommandRequest.processName)
}
inline void wsProcessCommandRequest::set_processname(const char* value) {
  set_has_processname();
  if (processname_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    processname_ = new ::std::string;
  }
  processname_->assign(value);
  // @@protoc_insertion_point(field_set_char:psmweb.wsProcessCommandRequest.processName)
}
inline void wsProcessCommandRequest::set_processname(const char* value, size_t size) {
  set_has_processname();
  if (processname_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    processname_ = new ::std::string;
  }
  processname_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:psmweb.wsProcessCommandRequest.processName)
}
inline ::std::string* wsProcessCommandRequest::mutable_processname() {
  set_has_processname();
  if (processname_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    processname_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:psmweb.wsProcessCommandRequest.processName)
  return processname_;
}
inline ::std::string* wsProcessCommandRequest::release_processname() {
  clear_has_processname();
  if (processname_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = processname_;
    processname_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void wsProcessCommandRequest::set_allocated_processname(::std::string* processname) {
  if (processname_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete processname_;
  }
  if (processname) {
    set_has_processname();
    processname_ = processname;
  } else {
    clear_has_processname();
    processname_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:psmweb.wsProcessCommandRequest.processName)
}

// -------------------------------------------------------------------

// swProcessCommandResponse

// required int32 token = 1;
inline bool swProcessCommandResponse::has_token() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void swProcessCommandResponse::set_has_token() {
  _has_bits_[0] |= 0x00000001u;
}
inline void swProcessCommandResponse::clear_has_token() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void swProcessCommandResponse::clear_token() {
  token_ = 0;
  clear_has_token();
}
inline ::google::protobuf::int32 swProcessCommandResponse::token() const {
  // @@protoc_insertion_point(field_get:psmweb.swProcessCommandResponse.token)
  return token_;
}
inline void swProcessCommandResponse::set_token(::google::protobuf::int32 value) {
  set_has_token();
  token_ = value;
  // @@protoc_insertion_point(field_set:psmweb.swProcessCommandResponse.token)
}

// required .psmweb.ProcessCommandType type = 2;
inline bool swProcessCommandResponse::has_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void swProcessCommandResponse::set_has_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void swProcessCommandResponse::clear_has_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void swProcessCommandResponse::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::psmweb::ProcessCommandType swProcessCommandResponse::type() const {
  // @@protoc_insertion_point(field_get:psmweb.swProcessCommandResponse.type)
  return static_cast< ::psmweb::ProcessCommandType >(type_);
}
inline void swProcessCommandResponse::set_type(::psmweb::ProcessCommandType value) {
  assert(::psmweb::ProcessCommandType_IsValid(value));
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:psmweb.swProcessCommandResponse.type)
}

// required .psmweb.swProcessCommandResponse.Result result = 3;
inline bool swProcessCommandResponse::has_result() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void swProcessCommandResponse::set_has_result() {
  _has_bits_[0] |= 0x00000004u;
}
inline void swProcessCommandResponse::clear_has_result() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void swProcessCommandResponse::clear_result() {
  result_ = 0;
  clear_has_result();
}
inline ::psmweb::swProcessCommandResponse_Result swProcessCommandResponse::result() const {
  // @@protoc_insertion_point(field_get:psmweb.swProcessCommandResponse.result)
  return static_cast< ::psmweb::swProcessCommandResponse_Result >(result_);
}
inline void swProcessCommandResponse::set_result(::psmweb::swProcessCommandResponse_Result value) {
  assert(::psmweb::swProcessCommandResponse_Result_IsValid(value));
  set_has_result();
  result_ = value;
  // @@protoc_insertion_point(field_set:psmweb.swProcessCommandResponse.result)
}

// optional string failReason = 4;
inline bool swProcessCommandResponse::has_failreason() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void swProcessCommandResponse::set_has_failreason() {
  _has_bits_[0] |= 0x00000008u;
}
inline void swProcessCommandResponse::clear_has_failreason() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void swProcessCommandResponse::clear_failreason() {
  if (failreason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    failreason_->clear();
  }
  clear_has_failreason();
}
inline const ::std::string& swProcessCommandResponse::failreason() const {
  // @@protoc_insertion_point(field_get:psmweb.swProcessCommandResponse.failReason)
  return *failreason_;
}
inline void swProcessCommandResponse::set_failreason(const ::std::string& value) {
  set_has_failreason();
  if (failreason_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    failreason_ = new ::std::string;
  }
  failreason_->assign(value);
  // @@protoc_insertion_point(field_set:psmweb.swProcessCommandResponse.failReason)
}
inline void swProcessCommandResponse::set_failreason(const char* value) {
  set_has_failreason();
  if (failreason_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    failreason_ = new ::std::string;
  }
  failreason_->assign(value);
  // @@protoc_insertion_point(field_set_char:psmweb.swProcessCommandResponse.failReason)
}
inline void swProcessCommandResponse::set_failreason(const char* value, size_t size) {
  set_has_failreason();
  if (failreason_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    failreason_ = new ::std::string;
  }
  failreason_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:psmweb.swProcessCommandResponse.failReason)
}
inline ::std::string* swProcessCommandResponse::mutable_failreason() {
  set_has_failreason();
  if (failreason_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    failreason_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:psmweb.swProcessCommandResponse.failReason)
  return failreason_;
}
inline ::std::string* swProcessCommandResponse::release_failreason() {
  clear_has_failreason();
  if (failreason_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = failreason_;
    failreason_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void swProcessCommandResponse::set_allocated_failreason(::std::string* failreason) {
  if (failreason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete failreason_;
  }
  if (failreason) {
    set_has_failreason();
    failreason_ = failreason;
  } else {
    clear_has_failreason();
    failreason_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:psmweb.swProcessCommandResponse.failReason)
}

// -------------------------------------------------------------------

// wsHealthCheck

// -------------------------------------------------------------------

// swHealthAck


// @@protoc_insertion_point(namespace_scope)

}  // namespace psmweb

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::psmweb::swProcessCommandResponse_Result> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::psmweb::swProcessCommandResponse_Result>() {
  return ::psmweb::swProcessCommandResponse_Result_descriptor();
}
template <> struct is_proto_enum< ::psmweb::psmType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::psmweb::psmType>() {
  return ::psmweb::psmType_descriptor();
}
template <> struct is_proto_enum< ::psmweb::ProcessCommandType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::psmweb::ProcessCommandType>() {
  return ::psmweb::ProcessCommandType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_WebProtocol_2eproto__INCLUDED
