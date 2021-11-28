import { VideoMetadata } from './VideoMetadata';
import { Box, Slide } from '@mui/material';
import { ThumbnailPaper } from './ThumbnailPaper';
import React from 'react';
import TouchRipple, { TouchRippleActions } from '@mui/material/ButtonBase/TouchRipple';
import { useMutation } from 'react-query';

export function VideoEntry(props: { metadata: VideoMetadata }) {
    const rippleRef = React.useRef<TouchRippleActions>();

    const { mutate } = useMutation(() => {
        return fetch('/videos/' + props.metadata.filename + ':play', {
            method: 'POST',
        });
    });
    // TouchRipple is not publicly documented, let's hope it doesn't break!

    return (
        <Slide direction="up" in={true}>
            <ThumbnailPaper
                thumbnailUrl={props.metadata.thumbnailUrl ?? 'https://i.imgur.com/C3QGaPB.jpeg'}
                className="actionable"
            >
                <div
                    onMouseDown={(e) => {
                        rippleRef.current?.start(e);
                    }}
                    onMouseUp={(e) => {
                        rippleRef.current?.stop(e);
                    }}
                    style={{
                        position: 'relative',
                    }}
                >
                    <Box sx={{ padding: '2em' }} onClick={() => mutate()}>
                        <TouchRipple ref={rippleRef} center={false} />
                        {props.metadata.filename} ({props.metadata.duration ?? 'Duration unknown'})
                    </Box>
                </div>
            </ThumbnailPaper>
        </Slide>
    );
}
