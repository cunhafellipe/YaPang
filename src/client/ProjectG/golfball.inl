// Original: d:\build\custom\temp\code\client\projectg\golfball.inl
// Inline members of CGolfBall, included at the end of golfball.h.

// golfball.inl:2, the ball singleton. Plain inline in every unit: the
// out-of-line copy split into SharedDoc is the COMDAT the units emit (its
// static's dtor thunk keeps the inline-static name ?me@?1??GolfBall...@$AV1@A).
// The guarded static takes one slot of cl's _$E counter in every unit that
// emits it.
inline CGolfBall &GolfBall(void) {
	static CGolfBall me;
	return me;
}

// golfball.inl:8
GOLFRULE_GOLFBALL_INLINE const CGolfBall::sExpected* CGolfBall::PreData(int index) const {
	if (index < 0 || index > m_preDataCount) {
		return 0;
	}

	if (index < 1500) {
		return &m_preData[index];
	}
	return &m_pPreDataOverflow[index - 1500];
}

// golfball.inl:25
// The point at index, to be written: up to m_preDataCount + 1 (the next
// one). Past the 1500 fixed points the overflow buffer grows by blocks of
// 100 points, copied over and zeroed, when the next point needs it.
GOLFRULE_GOLFBALL_INLINE CGolfBall::sExpected* CGolfBall::PreDataToWrite(int index) {
	if (index < 0 || index > m_preDataCount) {
		return 0;
	}

	if (index < 1500) {
		return &m_preData[index];
	}

	index -= 1500;
	if (index == m_preDataOverflowBlocks * 100) {
		m_preDataOverflowBlocks++;

		if (index == 0) {
			m_pPreDataOverflow = new sExpected[100];
			memset(m_pPreDataOverflow, 0, 100 * sizeof(sExpected));
		} else {
			sExpected *saved = new sExpected[index];
			memcpy(saved, m_pPreDataOverflow, index * sizeof(sExpected));

			delete[] m_pPreDataOverflow;
			m_pPreDataOverflow = new sExpected[index + 100];
			memcpy(m_pPreDataOverflow, saved, index * sizeof(sExpected));
			memset(m_pPreDataOverflow + index, 0, 100 * sizeof(sExpected));

			delete[] saved;
		}
	}

	return &m_pPreDataOverflow[index];
}

// golfball.inl:60
inline void CGolfBall::operator=(const CGolfBall &other) {
	if (m_pPreDataOverflow) {
		delete[] m_pPreDataOverflow;
		m_pPreDataOverflow = 0;
	}

	memcpy(this, &other, sizeof(CGolfBall));
	if (other.m_pPreDataOverflow) {
		sExpected *overflow = new sExpected[other.m_preDataOverflowBlocks * 100];
		memcpy(overflow, other.m_pPreDataOverflow, other.m_preDataOverflowBlocks * 100 * sizeof(sExpected));
		m_pPreDataOverflow = overflow;
	}
}

// golfball.inl:80 (out-of-line copy in SharedDoc's object)
inline CGolfBall::~CGolfBall(void) {
	if (m_pPreDataOverflow) {
		delete[] m_pPreDataOverflow;
		m_pPreDataOverflow = 0;
	}
}
