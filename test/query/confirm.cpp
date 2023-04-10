/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../test.hpp"
#include "../mocks/blocks.hpp"
#include "../mocks/chunk_store.hpp"

struct query_confirm_setup_fixture
{
    DELETE_COPY_MOVE(query_confirm_setup_fixture);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

    query_confirm_setup_fixture() NOEXCEPT
    {
        BOOST_REQUIRE(test::clear(test::directory));
    }

    ~query_confirm_setup_fixture() NOEXCEPT
    {
        BOOST_REQUIRE(test::clear(test::directory));
    }

    BC_POP_WARNING()
};

BOOST_FIXTURE_TEST_SUITE(query_confirm_tests, query_confirm_setup_fixture)

// nop event handler.
const auto events = [](auto, auto) {};

// This asserts.
////BOOST_AUTO_TEST_CASE(query_confirm__pop__zero__false)
////{
////    settings settings{};
////    settings.path = TEST_DIRECTORY;
////    test::chunk_store store{ settings };
////    test::query_accessor query{ store };
////    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
////    BOOST_REQUIRE(!query.pop_candidate());
////    BOOST_REQUIRE(!query.pop_confirmed());
////}

BOOST_AUTO_TEST_CASE(query_confirm__is_candidate_block__push_pop_candidate__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.is_candidate_block(0));
    BOOST_REQUIRE(!query.is_candidate_block(1));
    BOOST_REQUIRE(!query.is_candidate_block(2));

    BOOST_REQUIRE(query.push_candidate(1));
    BOOST_REQUIRE(query.is_candidate_block(1));

    BOOST_REQUIRE(query.push_candidate(2));
    BOOST_REQUIRE(query.is_candidate_block(2));

    BOOST_REQUIRE(query.pop_candidate());
    BOOST_REQUIRE(query.is_candidate_block(0));
    BOOST_REQUIRE(query.is_candidate_block(1));
    BOOST_REQUIRE(!query.is_candidate_block(2));

    BOOST_REQUIRE(query.pop_candidate());
    BOOST_REQUIRE(query.is_candidate_block(0));
    BOOST_REQUIRE(!query.is_candidate_block(1));
    BOOST_REQUIRE(!query.is_candidate_block(2));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_confirmed_block__push_pop_confirmed__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.is_confirmed_block(0));
    BOOST_REQUIRE(!query.is_confirmed_block(1));
    BOOST_REQUIRE(!query.is_confirmed_block(2));

    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(query.is_confirmed_block(1));

    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(query.is_confirmed_block(2));

    BOOST_REQUIRE(query.pop_confirmed());
    BOOST_REQUIRE(query.is_confirmed_block(0));
    BOOST_REQUIRE(query.is_confirmed_block(1));
    BOOST_REQUIRE(!query.is_confirmed_block(2));

    BOOST_REQUIRE(query.pop_confirmed());
    BOOST_REQUIRE(query.is_confirmed_block(0));
    BOOST_REQUIRE(!query.is_confirmed_block(1));
    BOOST_REQUIRE(!query.is_confirmed_block(2));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_confirmed_tx__confirm__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.is_confirmed_tx(0));
    BOOST_REQUIRE(!query.is_confirmed_tx(1));
    BOOST_REQUIRE(!query.is_confirmed_tx(2));

    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(!query.is_confirmed_tx(1));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_confirmed_tx(1));

    // Ordering between strong and confirmed is unimportant.
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(!query.is_confirmed_tx(2));
    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(query.is_confirmed_tx(2));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_confirmed_input__confirm__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));

    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(0, 0)));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(1, 0)));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(2, 0)));

    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(1, 0)));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(1, 0)));

    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(2, 0)));
    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(2, 0)));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_confirmed_output__confirm__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));

    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(0, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(1, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(2, 0)));

    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(1, 0)));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(1, 0)));

    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(2, 0)));
    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(2, 0)));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_spent_output__genesis__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(0, 0)));
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(1, 1)));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_spent_output__strong_confirmed__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2a, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.set(test::block3a, context{ 0, 3, 0 }));
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(0, 0))); // genesis
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(1, 0))); // block1a
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(1, 1))); // block1a
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(query.set_strong(3));
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(0, 0))); // genesis
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(1, 0))); // block1a
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(1, 1))); // block1a
    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(query.push_confirmed(3));
    BOOST_REQUIRE(!query.is_spent_output(query.to_output(0, 0))); // genesis
    BOOST_REQUIRE(query.is_spent_output(query.to_output(1, 0)));  // block1a
    BOOST_REQUIRE(query.is_spent_output(query.to_output(1, 1)));  // block1a
}

BOOST_AUTO_TEST_CASE(query_confirm__is_strong__strong__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.is_strong(query.to_input(0, 0)));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_strong__weak__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{}));
    BOOST_REQUIRE(!query.is_strong(query.to_input(1, 0)));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_strong(query.to_input(1, 0)));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_spent__unspent__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{}));
    BOOST_REQUIRE(!query.is_spent(query.to_input(0, 0))); // unspendable
    BOOST_REQUIRE(!query.is_spent(query.to_input(1, 0))); // unspent
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // non-existent
}

BOOST_AUTO_TEST_CASE(query_confirm__is_spent__unconfirmed_double_spend__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    BOOST_REQUIRE(query.set(test::block1a, context{}));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(!query.is_spent(query.to_input(0, 0))); // unspendable
    BOOST_REQUIRE(!query.is_spent(query.to_input(1, 0))); // spent by self (and missing)
    BOOST_REQUIRE(!query.is_spent(query.to_input(1, 1))); // spent by self (and missing)
    BOOST_REQUIRE(!query.is_spent(query.to_input(1, 2))); // spent by self (and missing)
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // missing tx
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1))); // missing tx

    BOOST_REQUIRE(query.set(test::block2a, context{}));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // unconfirmed self
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1))); // unconfirmed self

    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(query.push_confirmed(2));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // confirmed self
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1))); // confirmed self

    BOOST_REQUIRE(query.set(test::tx4));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // confirmed self, unconfirmed
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1))); // confirmed self, unconfirmed

    BOOST_REQUIRE(query.set(test::block3a, context{}));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0))); // confirmed self, unconfirmed(2)
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1))); // confirmed self, unconfirmed(2)

    BOOST_REQUIRE(query.set_strong(3));
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 0)));  // confirmed self, unconfirmed, confirmed (double spent)
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 1)));  // confirmed self, unconfirmed, confirmed (double spent)

    BOOST_REQUIRE(query.set_unstrong(2));
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 0)));  // unconfirmed self, unconfirmed, confirmed (spent)
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 1)));  // unconfirmed self, unconfirmed, confirmed (spent)

    BOOST_REQUIRE(query.set_unstrong(3));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0)));  // unconfirmed self, unconfirmed, unconfirmed (unspent)
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1)));  // unconfirmed self, unconfirmed, unconfirmed (unspent)

    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 0)));  // confirmed self, unconfirmed, confirmed (unspent)
    BOOST_REQUIRE(!query.is_spent(query.to_input(2, 1)));  // confirmed self, unconfirmed, confirmed (unspent)
}

BOOST_AUTO_TEST_CASE(query_confirm__is_spent__confirmed_double_spend__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1a, context{}));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.set(test::block2a, context{}));
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE(query.set(test::block3a, context{}));
    BOOST_REQUIRE(query.set_strong(3));
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 0))); // confirmed self and confirmed (double spent)
    BOOST_REQUIRE(query.is_spent(query.to_input(2, 1))); // confirmed self and confirmed (double spent)
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__spend_genesis__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::tx_spend_genesis));
    BOOST_REQUIRE(!query.is_mature(query.to_input(1, 0), 0));
    BOOST_REQUIRE(!query.is_mature(query.to_input(1, 0), 100));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__not_found__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(!query.is_mature(query.to_input(0, 1), 0));
    BOOST_REQUIRE(!query.is_mature(query.to_input(42, 24), 1000));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__null_input__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.is_mature(query.to_input(0, 0), 0));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__non_coinbase_strong_above__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::tx4));

    // Is not actually mature at height zero, but strong is presumed to always
    // be set at the current height and never above it (set above in this test).
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_mature(query.to_input(2, 0), 0));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__non_coinbase__true)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::tx4));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.is_mature(query.to_input(2, 0), 1));
}

BOOST_AUTO_TEST_CASE(query_confirm__is_mature__coinbase__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1b, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.set(test::tx2b));
    BOOST_REQUIRE(!query.is_mature(query.to_input(2, 0), 100));
    BOOST_REQUIRE(query.is_mature(query.to_input(2, 0), 101));
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__bad_link__integrity)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), error::integrity);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__null_points__block_success)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{}));
    BOOST_REQUIRE(query.set(test::block2, context{}));
    BOOST_REQUIRE(query.set(test::block3, context{}));

    // block1/2/3 at links 1/2/3 confirming at heights 1/2/3.
    // blocks have only coinbase txs, no need to be strong to be confirmable.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, true), system::error::block_success);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), system::error::block_success);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(3, true), system::error::block_success);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__missing_prevouts__integrity)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));

    // block1a is missing all three input prevouts.
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, true), database::error::integrity);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__spend_gensis__coinbase_maturity)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block_spend_genesis spends the genesis output.
    BOOST_REQUIRE(query.set(test::block_spend_genesis, context{ 0, 101, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // 1 + 100 = 101 (maturity, except genesis)
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, false), system::error::coinbase_maturity);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, true), system::error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__immature_prevouts__coinbase_maturity)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1b has only a coinbase tx.
    BOOST_REQUIRE(query.set(test::block1b, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, true), system::error::block_success);

    // block2b prematurely spends block1b's coinbase outputs.
    BOOST_REQUIRE(query.set(test::block2b, context{ 0, 100, 0 }));
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::coinbase_maturity);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__mature_prevouts__block_success)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1b has only a coinbase tx.
    BOOST_REQUIRE(query.set(test::block1b, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(1, true), system::error::block_success);

    // block2b spends block1b's coinbase outputs.
    BOOST_REQUIRE(query.set(test::block2b, context{ 0, 101, 0 }));
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::block_success);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), system::error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__spend_non_coinbase__block_success)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1a has non-coinbase tx/outputs.
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // block_spend_1a spends both block1a outputs.
    BOOST_REQUIRE(query.set(test::block_spend_1a, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.set_strong(2));

    // Maturity applies only to coinbase prevouts.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::block_success);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), system::error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__spend_coinbase_and_internal_immature__coinbase_maturity)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1b has coinbase tx/outputs.
    BOOST_REQUIRE(query.set(test::block1b, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // block_spend_internal_2b spends first block1a output and first own output.
    BOOST_REQUIRE(query.set(test::block_spend_internal_2b, context{ 0, 100, 0 }));
    ////BOOST_REQUIRE(query.set_strong(2));

    // Not confirmable because lack of maturity.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::coinbase_maturity);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), system::error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__spend_coinbase_and_internal_mature__block_success)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1b has coinbase tx/outputs.
    BOOST_REQUIRE(query.set(test::block1b, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // block_spend_internal_2b spends first block1a output and first own output.
    BOOST_REQUIRE(query.set(test::block_spend_internal_2b, context{ 0, 101, 0 }));
    ////BOOST_REQUIRE(query.set_strong(2));

    // Not confirmable because own block prevout is not strong.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::unconfirmed_spend);

    // block1b coinbase prevout: 1 + 100 = 101 (maturity)
    BOOST_REQUIRE(query.set_strong(2));
    BOOST_REQUIRE_EQUAL( query.block_confirmable(2, false), system::error::block_success);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__confirmed_double_spend__confirmed_double_spend)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1a has non-coinbase tx/outputs.
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // block2a spends both block1a outputs (though not itself confirmable is set strong).
    BOOST_REQUIRE(query.set(test::block2a, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.set_strong(2));

    // block_spend_1a (also) spends both block1a outputs (and is otherwise confirmable).
    BOOST_REQUIRE(query.set(test::block_spend_1a, context{ 0, 3, 0 }));
    BOOST_REQUIRE(query.set_strong(3));

    // Not confirmable because of intervening block2a implies double spend.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(3, true), system::error::confirmed_double_spend);
}

BOOST_AUTO_TEST_CASE(query_confirm__block_confirmable__unconfirmed_double_spend__block_success)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));

    // block1a has non-coinbase tx/outputs.
    BOOST_REQUIRE(query.set(test::block1a, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set_strong(1));

    // tx5 spends first block1a output.
    BOOST_REQUIRE(query.set(test::tx5));

    // block_spend_1a (also) spends both block1a outputs.
    BOOST_REQUIRE(query.set(test::block_spend_1a, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.set_strong(2));

    // Confirmable because of intervening tx5 is unconfirmed double spend.
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, false), system::error::block_success);
    BOOST_REQUIRE_EQUAL(query.block_confirmable(2, true), system::error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(query_confirm__set_strong__unassociated__false)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1.header(), context{}));
    BOOST_REQUIRE(!query.set_strong(1));
    BOOST_REQUIRE(!query.set_unstrong(1));
}

BOOST_AUTO_TEST_CASE(query_confirm__set_strong__set_unstrong__expected)
{
    settings settings{};
    settings.path = TEST_DIRECTORY;
    test::chunk_store store{ settings };
    test::query_accessor query{ store };
    BOOST_REQUIRE_EQUAL(store.create(events), error::success);
    BOOST_REQUIRE(query.initialize(test::genesis));
    BOOST_REQUIRE(query.set(test::block1, context{ 0, 1, 0 }));
    BOOST_REQUIRE(query.set(test::block2, context{ 0, 2, 0 }));
    BOOST_REQUIRE(query.push_confirmed(1));
    BOOST_REQUIRE(query.push_confirmed(2));

    BOOST_REQUIRE(query.is_confirmed_tx(0));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(0, 0)));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(0, 0)));

    BOOST_REQUIRE(!query.is_confirmed_tx(1));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(1, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(1, 0)));

    BOOST_REQUIRE(!query.is_confirmed_tx(2));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(2, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(2, 0)));

    BOOST_REQUIRE(query.set_strong(1));
    BOOST_REQUIRE(query.set_strong(2));

    BOOST_REQUIRE(query.is_confirmed_tx(0));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(0, 0)));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(0, 0)));

    BOOST_REQUIRE(query.is_confirmed_tx(1));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(1, 0)));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(1, 0)));

    BOOST_REQUIRE(query.is_confirmed_tx(2));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(2, 0)));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(2, 0)));

    BOOST_REQUIRE(query.set_unstrong(1));
    BOOST_REQUIRE(query.set_unstrong(2));

    BOOST_REQUIRE(query.is_confirmed_tx(0));
    BOOST_REQUIRE(query.is_confirmed_input(query.to_input(0, 0)));
    BOOST_REQUIRE(query.is_confirmed_output(query.to_output(0, 0)));

    BOOST_REQUIRE(!query.is_confirmed_tx(1));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(1, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(1, 0)));

    BOOST_REQUIRE(!query.is_confirmed_tx(2));
    BOOST_REQUIRE(!query.is_confirmed_input(query.to_input(2, 0)));
    BOOST_REQUIRE(!query.is_confirmed_output(query.to_output(2, 0)));
}

BOOST_AUTO_TEST_SUITE_END()
