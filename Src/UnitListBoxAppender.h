//---------------------------------------------------------------------------

#ifndef UnitListBoxAppenderH
#define UnitListBoxAppenderH
//---------------------------------------------------------------------------

#include <log4cpp/Portability.hh>
#include <log4cpp/LayoutAppender.hh>

namespace log4cpp {

    /**
	 * ListboxAppender appends LoggingEvents to TListBox.
     **/
	class ListboxAppender : public LayoutAppender {
	private:
		TListBox *FListBox;
	public:
		ListboxAppender(const std::string& name, TListBox *AListBox);
		virtual ~ListboxAppender();

        virtual bool reopen();
        virtual void close();

	protected:
		virtual void _append(const LoggingEvent& event);

    };
}

#endif
