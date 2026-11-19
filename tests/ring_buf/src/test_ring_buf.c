/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Un buffer nuevo debe estar vacio");
	zassert_equal(rb_count(), 0, "La cuenta de un buffer nuevo debe ser 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* TODO(l8-task1): Push a value, call rb_init(4) again, then
	 * verify the buffer is empty and count is 0.
	 * See TEST_SPEC.md "Suite ring_buf_init" #2.
	 */
	zassert_equal(rb_push(99), 0, "el push debe ser ok");
	zassert_equal(rb_count(), 1, " el buffer debe agregar un item");

	rb_init(4);

	zassert_true(rb_is_empty(), "reinit debe vaciar el buffer");
	zassert_equal(rb_count(), 0, "reinit debe resetear la cuenta");

	//ztest_test_skip();
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	/* TODO(l8-task1): rb_push(42), rb_pop(&v) -> v == 42, buffer empty after.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #1.
	 */
	int v;

	zassert_equal(rb_push(42), 0, "push debe ser ok");
	zassert_equal(rb_pop(&v), 0, "pop debe ser ok");
	zassert_equal(v, 42, "pop debe retornar el valor must pusheado");
	zassert_true(rb_is_empty(), "el buffer debe vaciarse despues del pop");

	//ztest_test_skip();
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	/* TODO(l8-task1): rb_push(1), rb_push(2), rb_push(3); pop three times
	 * and verify the values come out as 1, 2, 3 in that order.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #2.
	 */
	int v;

	zassert_equal(rb_push(1), 0, "push 1 debe ser OK");
	zassert_equal(rb_push(2), 0, "push 2 debe ser OK");
	zassert_equal(rb_push(3), 0, "push 3 debe ser OK");

	zassert_equal(rb_pop(&v), 0, "el primer pop debe ser OK");
	zassert_equal(v, 1, "primer pop debe coincidir con el valor del primer pusheado");
	zassert_equal(rb_pop(&v), 0, "segundo pop debe ser OK");
	zassert_equal(v, 2, "el segundo pop debe coincidir con el valor del segundo pusheado");
	zassert_equal(rb_pop(&v), 0, "el tercer pop debe ser OK");
	zassert_equal(v, 3, "el tercer pop debe coincidir con el valor del tercer pusheado");
	zassert_true(rb_is_empty(), "se debe vaciar el buffer despues de los tres pop");

	//ztest_test_skip();
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* TODO(l8-task1): Fill the buffer to its capacity of 4, then push
	 * one more value -> -ENOSPC.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #3.
	 */
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "pusheo %d debe ser OK", i);
	}

	zassert_true(rb_is_full(), "el buffer se debe haber llenado con 4 elementos");
	zassert_equal(rb_push(99), -ENOSPC, "un pusheo cuando este lleno debe devolver -ENOSPC");
	zassert_equal(rb_count(), 4, "el push rechazado no tiene que agregarse a la FIFO");

	//ztest_test_skip();
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	/* TODO(l8-task1): rb_push(7); rb_peek(&v) -> v == 7; rb_peek(&v) again
	 * -> v == 7; rb_count() still == 1.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #1.
	 */
	int v;

	zassert_equal(rb_push(7), 0, "pusheo debe ser OK");

	zassert_equal(rb_peek(&v), 0, "el primer peek debe ser OK");
	zassert_equal(v, 7, "el primer peek debe mostrar el valor del primer push");
	zassert_equal(rb_peek(&v), 0, "el segundo peek debe ser OK");
	zassert_equal(v, 7, "el segundo peek debe mostrar el mismo valor");
	zassert_equal(rb_count(), 1, "el peek no tiene que consumir la FIFO");
	 
	//ztest_test_skip();
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* TODO(l8-task1): rb_pop(NULL) -> -EINVAL.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #2.
	 */
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) debe retornar -EINVAL");

	//ztest_test_skip();
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* TODO(l8-task1): push 4 values -> rb_is_full() == true, rb_count() == 4.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #3.
	 */
	for (int i = 1; i <= 4; i++) {
		zassert_equal(rb_push(i), 0, "pusheo %d debe ser OK", i);
	}

	zassert_true(rb_is_full(), "el buffer debe estar lleno");
	zassert_equal(rb_count(), 4, "la cuenta debe ser igual a la capacidad");

	//ztest_test_skip();
}