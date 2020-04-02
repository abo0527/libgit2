#include "clar_libgit2.h"
#include "buffer.h"

static const char *test_string = "Have you seen that? Have you seeeen that??";

void test_buf_basic__resize(void)
{
	git_buf buf1 = GIT_BUF_INIT;
	git_buf_puts(&buf1, test_string);
	cl_assert(git_buf_oom(&buf1) == 0);
	cl_assert_equal_s(git_buf_cstr(&buf1), test_string);

	git_buf_puts(&buf1, test_string);
	cl_assert(strlen(git_buf_cstr(&buf1)) == strlen(test_string) * 2);
	git_buf_dispose(&buf1);
}

void test_buf_basic__resize_incremental(void)
{
	git_buf buf1 = GIT_BUF_INIT;

	/* Presently, asking for 6 bytes will round up to 8. */
	cl_git_pass(git_buf_puts(&buf1, "Hello"));
	cl_assert_equal_i(5, buf1.size);
	cl_assert_equal_i(8, buf1.asize);

	/* Ensure an additional byte does not realloc. */
	cl_git_pass(git_buf_grow_by(&buf1, 1));
	cl_assert_equal_i(5, buf1.size);
	cl_assert_equal_i(8, buf1.asize);

	/* But requesting many does. */
	cl_git_pass(git_buf_grow_by(&buf1, 16));
	cl_assert_equal_i(5, buf1.size);
	cl_assert(buf1.asize > 8);

	git_buf_dispose(&buf1);
}

void test_buf_basic__printf(void)
{
	git_buf buf2 = GIT_BUF_INIT;
	git_buf_printf(&buf2, "%s %s %d ", "shoop", "da", 23);
	cl_assert(git_buf_oom(&buf2) == 0);
	cl_assert_equal_s(git_buf_cstr(&buf2), "shoop da 23 ");

	git_buf_printf(&buf2, "%s %d", "woop", 42);
	cl_assert(git_buf_oom(&buf2) == 0);
	cl_assert_equal_s(git_buf_cstr(&buf2), "shoop da 23 woop 42");
	git_buf_dispose(&buf2);
}

void test_buf_basic__replace(void)
{
	git_buf buf = GIT_BUF_INIT;
	const char *replacements[][2] = {
		{ "%a", "aaaaaaaa" },
		{ "%b", "bbb" },
		{ "%c", "REPLACEMENT FOR C" }
	};

	git_buf_puts(&buf, "one two %a three four %b five six %c seven");
	git_buf_replace(&buf, replacements, 3);
	cl_assert_equal_s("one two aaaaaaaa three four bbb five six REPLACEMENT FOR C seven", buf.ptr);

	git_buf_dispose(&buf);
}

void test_buf_basic__shellquote(void)
{
	git_buf buf = GIT_BUF_INIT;

	cl_git_pass(git_buf_puts(&buf, "filename"));
	cl_git_pass(git_buf_shellquote(&buf));
	cl_assert_equal_s("\'filename\'", buf.ptr);

	git_buf_clear(&buf);

	cl_git_pass(git_buf_puts(&buf, "file name"));
	cl_git_pass(git_buf_shellquote(&buf));
	cl_assert_equal_s("\'file name\'", buf.ptr);

	git_buf_clear(&buf);

	cl_git_pass(git_buf_puts(&buf, "file\'name"));
	cl_git_pass(git_buf_shellquote(&buf));
	cl_assert_equal_s("\'file\'\\\'\'name\'", buf.ptr);

	git_buf_clear(&buf);

	cl_git_pass(git_buf_puts(&buf, "file!name"));
	cl_git_pass(git_buf_shellquote(&buf));
	cl_assert_equal_s("\'file\'\\!\'name\'", buf.ptr);

	git_buf_dispose(&buf);
}
