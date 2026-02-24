#include <gtest/gtest.h>

#include "vanadium/lib/lserver/MessageToken.h"

using namespace vanadium::lserver;

TEST(TokenPoolTest, AcquireClearsBuffer) {
  TokenPool pool(1);
  {
    auto token = pool.Acquire();
    token->buf = "Hello";
    ASSERT_TRUE(pool.Full());
  }
  {
    auto token = pool.Acquire();
    EXPECT_TRUE(token->buf.empty());
    EXPECT_TRUE(pool.Full());
  }
}

TEST(TokenPoolTest, TokenRAII) {
  TokenPool pool(1);
  EXPECT_FALSE(pool.Full());

  {
    auto token = pool.Acquire();
    EXPECT_TRUE(pool.Full());
  }

  EXPECT_FALSE(pool.Full());
}

TEST(TokenPoolTest, PooledStorageReuse) {
  TokenPool pool(1);
  ASSERT_FALSE(pool.Full());

  MessageToken* ptok;
  {
    auto token = pool.Acquire();
    ptok = &(*token);
    ASSERT_TRUE(pool.Full());
  }
  ASSERT_FALSE(pool.Full());

  auto token = pool.Acquire();
  ASSERT_TRUE(pool.Full());

  EXPECT_EQ(&(*token), ptok);
}

TEST(TokenPoolTest, MoveConstructorTransfersOwnership) {
  TokenPool pool(1);

  {
    PooledMessageToken moved_token;
    {
      auto token = pool.Acquire();
      moved_token = std::move(token);

      EXPECT_EQ(&(*token), nullptr);
      EXPECT_NE(&(*moved_token), nullptr);
      EXPECT_TRUE(pool.Full());
    }
    EXPECT_TRUE(pool.Full());
  }
  EXPECT_FALSE(pool.Full());
}

TEST(TokenPoolTest, MoveAssignmentTransfersOwnership) {
  TokenPool pool(2);

  auto token1 = pool.Acquire();
  auto token2 = pool.Acquire();
  EXPECT_TRUE(pool.Full());

  MessageToken* ptr1 = &(*token1);

  token2 = std::move(token1);
  EXPECT_FALSE(pool.Full());

  EXPECT_EQ(&(*token1), nullptr);
  EXPECT_EQ(&(*token2), ptr1);
}
