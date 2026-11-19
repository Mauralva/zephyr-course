# Coverage Analysis of Untested Functions

## 1. `rb_init()` — invalid capacity (lines 23–25)

```c
	if (capacity == 0U || capacity > RING_BUF_MAX_CAPACITY) {
		LOG_ERR("invalid capacity %u (max %u)", capacity, RING_BUF_MAX_CAPACITY);
		return -EINVAL;
```

No test calls `rb_init()` with an invalid argument like `0` or `> RING_BUF_MAX_CAPACITY`, every init uses capacity 4. To cover this we must test it and assert both return `-EINVAL`.

## 2. `rb_pop()` — pop on empty buffer (lines 56–58)

```c
	if (rb.count == 0U) {
		LOG_WRN("buffer empty");
		return -ENODATA;
```

No test pops from an empty buffer, every pop is done after a push, so the `-ENODATA` path never runs. To cover this we must pop on a fresh buffer and assert it returns `-ENODATA`.

## 3. `rb_peek()` — NULL pointer and empty buffer (lines 70–77)

```c
	if (value == NULL) {
		LOG_ERR("value pointer is NULL");
		return -EINVAL;
	}

	if (rb.count == 0U) {
		LOG_WRN("buffer empty");
		return -ENODATA;
	}
```

We test `rb_pop(NULL)` but never `rb_peek(NULL)`, so the NULL check for peek is not covered. Also, the only peek test pushes first, so the `-ENODATA` path for an empty buffer is not covered either. To cover both we must test `rb_peek(NULL)` expecting `-EINVAL`, and `rb_peek(&v)` on an empty buffer expecting `-ENODATA`.
