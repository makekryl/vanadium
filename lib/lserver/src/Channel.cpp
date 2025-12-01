#include "vanadium/lib/lserver/Channel.h"

#include <charconv>
#include <string_view>

#include "vanadium/lib/lserver/MessageToken.h"

namespace vanadium::lserver {

void Channel::Read() {
  auto token = pool_.Acquire();
  auto& buf = token->buf;

  buf.resize(128);
  transport_->ReadLine(buf);

  std::size_t message_size;
  {
    std::string_view sbuf{buf.data(), buf.size()};
    const auto separator_pos = sbuf.find(':');
    const auto length_fragment = sbuf.substr(separator_pos + 2, sbuf.length() - separator_pos - 2);
    std::from_chars(length_fragment.begin(), length_fragment.end(), message_size);
  }

  transport_->Read({buf.data(), buf.data() + 2});  // \r\n

  buf.resize(message_size);
  transport_->Read(buf);

  ready_.emplace(std::move(token));
}

void Channel::Write() {
  PooledMessageToken token;
  out_queue_.pop(token);

  transport_->Write("Content-Length: ");
  transport_->Write(std::to_string(token->buf.size()));
  transport_->Write("\r\n\r\n");
  transport_->Write(std::string_view{token->buf.data(), token->buf.size()});
  transport_->Flush();
}

void Channel::Enqueue(PooledMessageToken&& token) {
  out_queue_.emplace(std::move(token));
}

PooledMessageToken Channel::Poll() {
  PooledMessageToken token;
  ready_.pop(token);
  return token;
}

}  // namespace vanadium::lserver
