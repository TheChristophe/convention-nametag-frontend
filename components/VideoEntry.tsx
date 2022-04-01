import { VideoMetadata } from './VideoMetadata';
import { Box, Grid, Paper, Slide } from '@mui/material';
import React from 'react';
import TouchRipple, { TouchRippleActions } from '@mui/material/ButtonBase/TouchRipple';
import { useMutation } from 'react-query';
import { HOST } from './config';
import DeleteForeverIcon from '@mui/icons-material/DeleteForever';

export function Thumbnail({ thumbnailUrl }: { thumbnailUrl: string }) {
    return (
        <Box
            sx={{
                borderRight: '1px solid lightgray',
                height: '100%',
                backgroundImage: thumbnailUrl.length ? 'url(' + thumbnailUrl + ')' : undefined,
                backgroundSize: 'cover',
            }}
        />
    );
}

export function VideoEntry(props: { metadata: VideoMetadata }) {
    const rippleRef = React.useRef<TouchRippleActions>();

    const play = useMutation(() => {
        return fetch(HOST + '/videos/' + props.metadata.filename + '/play', {
            method: 'POST',
        });
    });

    const delete_ = useMutation(() => {
        return fetch(HOST + '/videos/' + props.metadata.filename, {
            method: 'DELETE',
        });
    });

    const thumbnailUrl = props.metadata.thumbnailUrl ?? 'https://i.imgur.com/C3QGaPB.jpeg';

    return (
        <Slide direction="up" in={true}>
            <Paper elevation={4}>
                <Grid container width="100%">
                    <Grid item xs={6} md={4}>
                        <Thumbnail thumbnailUrl={thumbnailUrl} />
                    </Grid>
                    <Grid item xs={6} md={8}>
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
                            <Grid container width="100%">
                                <Grid item xs={10} md={9} className="actionable">
                                    <Box padding="2em" onClick={() => play.mutate()}>
                                        {/*<TouchRipple ref={rippleRef} center={false} /> */}
                                        {props.metadata.filename}
                                        <br />({props.metadata.duration ?? '??:??'})
                                    </Box>
                                </Grid>
                                <Grid
                                    item
                                    xs={2}
                                    md={3}
                                    display="flex"
                                    flexDirection="row"
                                    justifyContent="center"
                                    borderLeft="1px solid lightgray"
                                >
                                    <Box
                                        py="2em"
                                        alignSelf="center"
                                        className="actionable"
                                        onClick={() => delete_.mutate()}
                                    >
                                        <DeleteForeverIcon />
                                    </Box>
                                </Grid>
                            </Grid>
                        </div>
                    </Grid>
                </Grid>
            </Paper>
        </Slide>
    );
}
