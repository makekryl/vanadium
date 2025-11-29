#include <gtest/gtest.h>

#include "vanadium/lib/jsonrpc/Server.h"

using namespace vanadium::lib::jsonrpc;

namespace stubs {
struct Context {
  int x{0};
};
struct Params {
  int a, b, c;
};
struct Result {
  Params p;
  int x, y, z;
};
}  // namespace stubs

TEST(ServerTest, RequestRegistration) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> stubs::Result {
    return {};
  };

  Server<stubs::Context> srv;
  EXPECT_FALSE(srv.IsBound("request"));

  srv.Bind<+handler>("request");
  EXPECT_TRUE(srv.IsBound("request"));

  srv.Unbind("request");
  EXPECT_FALSE(srv.IsBound("request"));
}

TEST(ServerTest, NotificationRegistration) {
  const auto handler = [](stubs::Context&, const stubs::Params&) -> void {};

  Server<stubs::Context> srv;
  EXPECT_FALSE(srv.IsBound("notification"));

  srv.Bind<+handler>("notification");
  EXPECT_TRUE(srv.IsBound("notification"));

  srv.Unbind("notification");
  EXPECT_FALSE(srv.IsBound("notification"));
}

TEST(ServerTest, RequestInvocation) {
  const auto handler = [](stubs::Context& ctx, const stubs::Params& params) -> stubs::Result {
    ctx.x = 42;
    return {.p = params, .x = 3, .y = 2, .z = 1};
  };

  stubs::Context ctx{.x = 0};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, R"({
    "jsonrpc": "2.0",
    "method": "example",
    "params": {
      "a": 1,
      "b": 2,
      "c": 3
    },
    "id": 1
  })");
  EXPECT_EQ(res, R"({"jsonrpc":"2.0","result":{"p":{"a":1,"b":2,"c":3},"x":3,"y":2,"z":1},"id":1})");
  EXPECT_EQ(ctx.x, 42);
}

TEST(ServerTest, NotificationInvocation) {
  const auto handler = [](stubs::Context& ctx, const stubs::Params& params) -> void {
    ctx.x = params.a + params.b + params.c;
  };

  stubs::Context ctx{.x = 0};
  std::string buf;

  Server<stubs::Context> srv;
  srv.Bind<+handler>("example");
  ASSERT_TRUE(srv.IsBound("example"));

  const auto res = srv.Call(ctx, buf, R"({
    "jsonrpc": "2.0",
    "method": "example",
    "params": {
      "a": 1,
      "b": 2,
      "c": 3
    }
  })");
  EXPECT_EQ(res, std::nullopt);
  EXPECT_EQ(ctx.x, 6);
}

// TODO: add tests for error cases
