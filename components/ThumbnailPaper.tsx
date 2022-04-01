import React, { ForwardedRef, ReactNode } from 'react';
import { Box, Grid, Paper } from '@mui/material';

export const ThumbnailPaper = React.forwardRef(function ThumbnailPaper(
    props: { thumbnailUrl: string; children: ReactNode; className?: string },
    ref: ForwardedRef<HTMLDivElement>
) {
    return (
        <div ref={ref}>
            <Paper elevation={4} className={props.className}>
                <Grid container width="100%">
                    <Grid item xs={6} md={4}>
                        <Box
                            sx={{
                                borderRight: '1px solid lightgray',
                                height: '100%',
                                backgroundImage: props.thumbnailUrl.length
                                    ? 'url(' + props.thumbnailUrl + ')'
                                    : undefined,
                                backgroundSize: 'cover',
                            }}
                        />
                    </Grid>
                    <Grid item xs={6} md={8}>
                        {props.children}
                    </Grid>
                </Grid>
            </Paper>
        </div>
    );
});
