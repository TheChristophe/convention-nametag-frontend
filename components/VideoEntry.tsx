import { Box, Grid, Paper, Slide } from '@mui/material';
import { useMutation } from '@tanstack/react-query';
import { HOST } from './config';
import DeleteForeverIcon from '@mui/icons-material/DeleteForever';
import type VideoMetadata from 'components/VideoMetadata';
import actionable from './actionable.module.css';

type ThumbnailProps = { thumbnailUrl: string };
export const Thumbnail = ({ thumbnailUrl }: ThumbnailProps) => (
    <Box
        sx={{
            borderRight: '1px solid lightgray',
            height: '100%',
            backgroundImage: thumbnailUrl.length ? 'url(' + thumbnailUrl + ')' : undefined,
            backgroundSize: 'cover',
        }}
    />
);

const humanReadableTime = (seconds: number) => {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = Math.floor(seconds % 60);
    return minutes.toString().padStart(2, '0') + ':' + remainingSeconds.toString().padStart(2, '0');
};

type VideoEntryProps = { metadata: VideoMetadata; reload: () => void };
const VideoEntry = ({ metadata, reload }: VideoEntryProps) => {
    const play = useMutation({
        mutationFn: () =>
            fetch(HOST + '/videos/' + metadata.filename + '/play', {
                method: 'POST',
            }),
    });

    const delete_ = useMutation({
        mutationFn: () =>
            fetch(HOST + '/videos/' + metadata.filename, {
                method: 'DELETE',
            }),
        onSuccess: () => {
            reload();
        },
    });

    const thumbnailUrl = metadata.thumbnailUrl ?? 'https://i.imgur.com/C3QGaPB.jpeg';

    return (
        <Slide direction="up" in={true}>
            <Paper elevation={4}>
                <Grid container width="100%">
                    <Grid item xs={6} md={4}>
                        <Thumbnail thumbnailUrl={thumbnailUrl} />
                    </Grid>
                    <Grid item xs={6} md={8}>
                        <div
                            style={{
                                position: 'relative',
                            }}
                        >
                            <Grid container width="100%">
                                <Grid item xs={10} md={9} className={actionable['actionable']}>
                                    <Box padding="2em" onClick={() => play.mutate()}>
                                        {metadata.filename}
                                        <br />(
                                        {metadata.duration
                                            ? humanReadableTime(metadata.duration)
                                            : '??:??'}
                                        )
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
                                        className={actionable['actionable']}
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
};

export default VideoEntry;
