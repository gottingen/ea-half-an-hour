// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: halakv/kv.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_halakv_2fkv_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_halakv_2fkv_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3020000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3020000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/service.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_halakv_2fkv_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_halakv_2fkv_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_halakv_2fkv_2eproto;
namespace halakv {
class KvRequest;
struct KvRequestDefaultTypeInternal;
extern KvRequestDefaultTypeInternal _KvRequest_default_instance_;
class KvResponse;
struct KvResponseDefaultTypeInternal;
extern KvResponseDefaultTypeInternal _KvResponse_default_instance_;
}  // namespace halakv
PROTOBUF_NAMESPACE_OPEN
template<> ::halakv::KvRequest* Arena::CreateMaybeMessage<::halakv::KvRequest>(Arena*);
template<> ::halakv::KvResponse* Arena::CreateMaybeMessage<::halakv::KvResponse>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace halakv {

// ===================================================================

class KvRequest final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:halakv.KvRequest) */ {
 public:
  inline KvRequest() : KvRequest(nullptr) {}
  ~KvRequest() override;
  explicit PROTOBUF_CONSTEXPR KvRequest(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  KvRequest(const KvRequest& from);
  KvRequest(KvRequest&& from) noexcept
    : KvRequest() {
    *this = ::std::move(from);
  }

  inline KvRequest& operator=(const KvRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline KvRequest& operator=(KvRequest&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const KvRequest& default_instance() {
    return *internal_default_instance();
  }
  static inline const KvRequest* internal_default_instance() {
    return reinterpret_cast<const KvRequest*>(
               &_KvRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(KvRequest& a, KvRequest& b) {
    a.Swap(&b);
  }
  inline void Swap(KvRequest* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(KvRequest* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  KvRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<KvRequest>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const KvRequest& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const KvRequest& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(KvRequest* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "halakv.KvRequest";
  }
  protected:
  explicit KvRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kKeyFieldNumber = 1,
    kValueFieldNumber = 2,
  };
  // required string key = 1;
  bool has_key() const;
  private:
  bool _internal_has_key() const;
  public:
  void clear_key();
  const std::string& key() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_key(ArgT0&& arg0, ArgT... args);
  std::string* mutable_key();
  PROTOBUF_NODISCARD std::string* release_key();
  void set_allocated_key(std::string* key);
  private:
  const std::string& _internal_key() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_key(const std::string& value);
  std::string* _internal_mutable_key();
  public:

  // optional string value = 2;
  bool has_value() const;
  private:
  bool _internal_has_value() const;
  public:
  void clear_value();
  const std::string& value() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_value(ArgT0&& arg0, ArgT... args);
  std::string* mutable_value();
  PROTOBUF_NODISCARD std::string* release_value();
  void set_allocated_value(std::string* value);
  private:
  const std::string& _internal_value() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_value(const std::string& value);
  std::string* _internal_mutable_value();
  public:

  // @@protoc_insertion_point(class_scope:halakv.KvRequest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr key_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr value_;
  friend struct ::TableStruct_halakv_2fkv_2eproto;
};
// -------------------------------------------------------------------

class KvResponse final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:halakv.KvResponse) */ {
 public:
  inline KvResponse() : KvResponse(nullptr) {}
  ~KvResponse() override;
  explicit PROTOBUF_CONSTEXPR KvResponse(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  KvResponse(const KvResponse& from);
  KvResponse(KvResponse&& from) noexcept
    : KvResponse() {
    *this = ::std::move(from);
  }

  inline KvResponse& operator=(const KvResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline KvResponse& operator=(KvResponse&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const KvResponse& default_instance() {
    return *internal_default_instance();
  }
  static inline const KvResponse* internal_default_instance() {
    return reinterpret_cast<const KvResponse*>(
               &_KvResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(KvResponse& a, KvResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(KvResponse* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(KvResponse* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  KvResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<KvResponse>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const KvResponse& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const KvResponse& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(KvResponse* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "halakv.KvResponse";
  }
  protected:
  explicit KvResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMessageFieldNumber = 2,
    kValueFieldNumber = 3,
    kCodeFieldNumber = 1,
  };
  // required string message = 2;
  bool has_message() const;
  private:
  bool _internal_has_message() const;
  public:
  void clear_message();
  const std::string& message() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_message(ArgT0&& arg0, ArgT... args);
  std::string* mutable_message();
  PROTOBUF_NODISCARD std::string* release_message();
  void set_allocated_message(std::string* message);
  private:
  const std::string& _internal_message() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_message(const std::string& value);
  std::string* _internal_mutable_message();
  public:

  // optional string value = 3;
  bool has_value() const;
  private:
  bool _internal_has_value() const;
  public:
  void clear_value();
  const std::string& value() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_value(ArgT0&& arg0, ArgT... args);
  std::string* mutable_value();
  PROTOBUF_NODISCARD std::string* release_value();
  void set_allocated_value(std::string* value);
  private:
  const std::string& _internal_value() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_value(const std::string& value);
  std::string* _internal_mutable_value();
  public:

  // required int32 code = 1;
  bool has_code() const;
  private:
  bool _internal_has_code() const;
  public:
  void clear_code();
  int32_t code() const;
  void set_code(int32_t value);
  private:
  int32_t _internal_code() const;
  void _internal_set_code(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:halakv.KvResponse)
 private:
  class _Internal;

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr message_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr value_;
  int32_t code_;
  friend struct ::TableStruct_halakv_2fkv_2eproto;
};
// ===================================================================

class KvService_Stub;

class KvService : public ::PROTOBUF_NAMESPACE_ID::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline KvService() {};
 public:
  virtual ~KvService();

  typedef KvService_Stub Stub;

  static const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* descriptor();

  virtual void set(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void get(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void remove(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);

  // implements Service ----------------------------------------------

  const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                  ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                  const ::PROTOBUF_NAMESPACE_ID::Message* request,
                  ::PROTOBUF_NAMESPACE_ID::Message* response,
                  ::google::protobuf::Closure* done);
  const ::PROTOBUF_NAMESPACE_ID::Message& GetRequestPrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const;
  const ::PROTOBUF_NAMESPACE_ID::Message& GetResponsePrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(KvService);
};

class KvService_Stub : public KvService {
 public:
  KvService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel);
  KvService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
                   ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership);
  ~KvService_Stub();

  inline ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel() { return channel_; }

  // implements KvService ------------------------------------------

  void set(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);
  void get(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);
  void remove(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::halakv::KvRequest* request,
                       ::halakv::KvResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(KvService_Stub);
};


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// KvRequest

// required string key = 1;
inline bool KvRequest::_internal_has_key() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool KvRequest::has_key() const {
  return _internal_has_key();
}
inline void KvRequest::clear_key() {
  key_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& KvRequest::key() const {
  // @@protoc_insertion_point(field_get:halakv.KvRequest.key)
  return _internal_key();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void KvRequest::set_key(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 key_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:halakv.KvRequest.key)
}
inline std::string* KvRequest::mutable_key() {
  std::string* _s = _internal_mutable_key();
  // @@protoc_insertion_point(field_mutable:halakv.KvRequest.key)
  return _s;
}
inline const std::string& KvRequest::_internal_key() const {
  return key_.Get();
}
inline void KvRequest::_internal_set_key(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  key_.Set(value, GetArenaForAllocation());
}
inline std::string* KvRequest::_internal_mutable_key() {
  _has_bits_[0] |= 0x00000001u;
  return key_.Mutable(GetArenaForAllocation());
}
inline std::string* KvRequest::release_key() {
  // @@protoc_insertion_point(field_release:halakv.KvRequest.key)
  if (!_internal_has_key()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = key_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (key_.IsDefault()) {
    key_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void KvRequest::set_allocated_key(std::string* key) {
  if (key != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  key_.SetAllocated(key, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (key_.IsDefault()) {
    key_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:halakv.KvRequest.key)
}

// optional string value = 2;
inline bool KvRequest::_internal_has_value() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool KvRequest::has_value() const {
  return _internal_has_value();
}
inline void KvRequest::clear_value() {
  value_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& KvRequest::value() const {
  // @@protoc_insertion_point(field_get:halakv.KvRequest.value)
  return _internal_value();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void KvRequest::set_value(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000002u;
 value_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:halakv.KvRequest.value)
}
inline std::string* KvRequest::mutable_value() {
  std::string* _s = _internal_mutable_value();
  // @@protoc_insertion_point(field_mutable:halakv.KvRequest.value)
  return _s;
}
inline const std::string& KvRequest::_internal_value() const {
  return value_.Get();
}
inline void KvRequest::_internal_set_value(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  value_.Set(value, GetArenaForAllocation());
}
inline std::string* KvRequest::_internal_mutable_value() {
  _has_bits_[0] |= 0x00000002u;
  return value_.Mutable(GetArenaForAllocation());
}
inline std::string* KvRequest::release_value() {
  // @@protoc_insertion_point(field_release:halakv.KvRequest.value)
  if (!_internal_has_value()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  auto* p = value_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (value_.IsDefault()) {
    value_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void KvRequest::set_allocated_value(std::string* value) {
  if (value != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  value_.SetAllocated(value, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (value_.IsDefault()) {
    value_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:halakv.KvRequest.value)
}

// -------------------------------------------------------------------

// KvResponse

// required int32 code = 1;
inline bool KvResponse::_internal_has_code() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool KvResponse::has_code() const {
  return _internal_has_code();
}
inline void KvResponse::clear_code() {
  code_ = 0;
  _has_bits_[0] &= ~0x00000004u;
}
inline int32_t KvResponse::_internal_code() const {
  return code_;
}
inline int32_t KvResponse::code() const {
  // @@protoc_insertion_point(field_get:halakv.KvResponse.code)
  return _internal_code();
}
inline void KvResponse::_internal_set_code(int32_t value) {
  _has_bits_[0] |= 0x00000004u;
  code_ = value;
}
inline void KvResponse::set_code(int32_t value) {
  _internal_set_code(value);
  // @@protoc_insertion_point(field_set:halakv.KvResponse.code)
}

// required string message = 2;
inline bool KvResponse::_internal_has_message() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool KvResponse::has_message() const {
  return _internal_has_message();
}
inline void KvResponse::clear_message() {
  message_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& KvResponse::message() const {
  // @@protoc_insertion_point(field_get:halakv.KvResponse.message)
  return _internal_message();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void KvResponse::set_message(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 message_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:halakv.KvResponse.message)
}
inline std::string* KvResponse::mutable_message() {
  std::string* _s = _internal_mutable_message();
  // @@protoc_insertion_point(field_mutable:halakv.KvResponse.message)
  return _s;
}
inline const std::string& KvResponse::_internal_message() const {
  return message_.Get();
}
inline void KvResponse::_internal_set_message(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  message_.Set(value, GetArenaForAllocation());
}
inline std::string* KvResponse::_internal_mutable_message() {
  _has_bits_[0] |= 0x00000001u;
  return message_.Mutable(GetArenaForAllocation());
}
inline std::string* KvResponse::release_message() {
  // @@protoc_insertion_point(field_release:halakv.KvResponse.message)
  if (!_internal_has_message()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = message_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (message_.IsDefault()) {
    message_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void KvResponse::set_allocated_message(std::string* message) {
  if (message != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  message_.SetAllocated(message, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (message_.IsDefault()) {
    message_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:halakv.KvResponse.message)
}

// optional string value = 3;
inline bool KvResponse::_internal_has_value() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool KvResponse::has_value() const {
  return _internal_has_value();
}
inline void KvResponse::clear_value() {
  value_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& KvResponse::value() const {
  // @@protoc_insertion_point(field_get:halakv.KvResponse.value)
  return _internal_value();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void KvResponse::set_value(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000002u;
 value_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:halakv.KvResponse.value)
}
inline std::string* KvResponse::mutable_value() {
  std::string* _s = _internal_mutable_value();
  // @@protoc_insertion_point(field_mutable:halakv.KvResponse.value)
  return _s;
}
inline const std::string& KvResponse::_internal_value() const {
  return value_.Get();
}
inline void KvResponse::_internal_set_value(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  value_.Set(value, GetArenaForAllocation());
}
inline std::string* KvResponse::_internal_mutable_value() {
  _has_bits_[0] |= 0x00000002u;
  return value_.Mutable(GetArenaForAllocation());
}
inline std::string* KvResponse::release_value() {
  // @@protoc_insertion_point(field_release:halakv.KvResponse.value)
  if (!_internal_has_value()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  auto* p = value_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (value_.IsDefault()) {
    value_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void KvResponse::set_allocated_value(std::string* value) {
  if (value != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  value_.SetAllocated(value, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (value_.IsDefault()) {
    value_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:halakv.KvResponse.value)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace halakv

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_halakv_2fkv_2eproto
