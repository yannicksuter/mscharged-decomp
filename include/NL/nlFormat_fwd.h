#ifndef NL_NLFORMAT_FWD_H
#define NL_NLFORMAT_FWD_H

template <typename StringType, typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6, typename T7>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6,
    const T7& t7);

template <typename StringType, typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6);

template <typename StringType, typename T0, typename T1, typename T2, typename T3,
    typename T4, typename T5>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2, const T3& t3, const T4& t4, const T5& t5);

template <typename StringType, typename T0, typename T1, typename T2, typename T3,
    typename T4>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2, const T3& t3, const T4& t4);

template <typename StringType, typename T0, typename T1, typename T2, typename T3>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2, const T3& t3);

template <typename StringType, typename T0, typename T1, typename T2>
StringType Format(const StringType& string, const T0& t0, const T1& t1,
    const T2& t2);

template <typename StringType, typename T0, typename T1>
StringType Format(const StringType& string, const T0& t0, const T1& t1);

template <typename StringType, typename T0>
StringType Format(const StringType& string, const T0& t0);

#endif // NL_NLFORMAT_FWD_H
