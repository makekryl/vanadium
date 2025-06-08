#include <string_view>

#include "LSChannel.h"
#include "LSMessageToken.h"

namespace vanadium::lsp {

void Channel::Read() {
  auto token = pool_->Acquire();
  auto& buf = token->buf;

  buf.resize(128);
  transport_->ReadLine(buf);

  std::size_t message_size;

  std::string_view bufsv{buf.data(), buf.size()};
  const auto separator_pos = bufsv.find(':');
  const auto svp = bufsv.substr(separator_pos + 2, bufsv.length() - separator_pos - 2);
  std::from_chars(svp.begin(), svp.end(), message_size);

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

}  // namespace vanadium::lsp
