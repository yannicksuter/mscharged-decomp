#ifndef REVOLUTION_HBM_ASSERT_HPP
#define REVOLUTION_HBM_ASSERT_HPP

/*******************************************************************************
 * NW4HBMAssert/NW4HBMCheck base macros
 */

#ifdef HBM_ASSERT
#define NW4HBMAssertMessage_FileLine(file_, line_, expr_, ...) \
    ((void)((expr_) || (::nw4hbm::db::Panic(file_, line_, __VA_ARGS__), 0)))

#define NW4HBMCheckMessage_FileLine(file_, line_, expr_, ...) \
    ((void)((expr_) || (::nw4hbm::db::Warning(file_, line_, __VA_ARGS__), 0)))
#else
#define NW4HBMAssertMessage_FileLine(file_, line_, expr_, ...) ((void)(0))
#define NW4HBMCheckMessage_FileLine(file_, line_, expr_, ...) ((void)(0))
#endif

/*******************************************************************************
 * Derived macros
 */

#define NW4HBMAssertMessage_Line(expr_, line_, ...) NW4HBMAssertMessage_FileLine(__FILE__, line_, expr_, __VA_ARGS__)
#define NW4HBMAssertMessage(expr_, ...) NW4HBMAssertMessage_FileLine(__FILE__, __LINE__, expr_, __VA_ARGS__)

#define NW4HBMAssert_FileLine(expr_, file_, line_) \
    NW4HBMAssertMessage_FileLine(file_, line_, expr_, "NW4HBM:Failed assertion " #expr_)

#define NW4HBMAssert_Line(expr_, line_) \
    NW4HBMAssertMessage_FileLine(__FILE__, line_, expr_, "NW4HBM:Failed assertion " #expr_)

#define NW4HBMAssert(expr_) NW4HBMAssertMessage_FileLine(__FILE__, __LINE__, expr_, "NW4HBM:Failed assertion " #expr_)

#define NW4HBMAssertPointerNonnull_Line(ptr_, line_) \
    NW4HBMAssertMessage_FileLine(__FILE__, line_, (ptr_) != 0, "NW4HBM:Pointer must not be NULL (" #ptr_ ")")

#define NW4HBMAssertPointerNonnull(ptr_) \
    NW4HBMAssertMessage_FileLine(__FILE__, __LINE__, (ptr_) != 0, "NW4HBM:Pointer must not be NULL (" #ptr_ ")")

#define NW4HBMCheckMessage_Line(expr_, line_, ...) NW4HBMCheckMessage_FileLine(__FILE__, line_, expr_, __VA_ARGS__)
#define NW4HBMCheckMessage(expr_, ...) NW4HBMCheckMessage_FileLine(__FILE__, __LINE__, expr_, __VA_ARGS__)

namespace nw4hbm {
namespace db {

extern void Panic(char const* file, int line, char const* msg, ...);
extern void Warning(char const* file, int line, char const* msg, ...);

} // namespace db
} // namespace nw4hbm

#endif
