#include <radiopixel_protocol.h>
#include "Pattern.h"
#include "Sequence.h"


ms_t millis();


class Player
{
public:
    Player()
        : sequence( NULL ), step( 0 ), stepTime( 0 ),
          pattern( NULL ), patternId( RadioPixel::Command::Gradient ), 
          lastUpdate( 0 ), speed( 35 )
    {
    }

    //! returns the current sequence
    Sequence *GetSequence( ) { return sequence; }

    //! Replace sequence
    void SetSequence( Sequence *_sequence );

    //! Advance the sequence via a button press
    void AdvanceSequence( );

    //! Returns the current command
    bool GetCommand( RadioPixel::Command *command );

    //! update to the next pattern in the sequence if needed
    // returns true if pattern changed, ie need to transmit
    bool UpdatePattern( ms_t now, Stripper *strip );

    //! update the strip with the current pattern if needed
    void UpdateStrip( ms_t now, Stripper *strip );

protected:
    Sequence *sequence;
    int step; // the current step index
    ms_t stepTime; // time we started the current step
    
    Pattern *pattern;
    uint8_t patternId;    
    ms_t lastUpdate;
    uint8_t speed;
};

const ms_t FRAME_MS = 1000 / 125;

